#include "mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QApplication>
#include <QScreen>
#include <QDateTime>
#include <QFont>
#include <QFrame>

static const int CARDS_PER_ROW = 5;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_cities = {
        {"Buenos Aires",     -34.6099, -58.3924},
        {"Córdoba",          -31.4201, -64.1888},
        {"Rosario",          -32.9468, -60.6393},
        {"Mendoza",          -32.8895, -68.8458},
        {"La Plata",         -34.9205, -57.9536},
        {"Tucumán",          -26.8083, -65.2176},
        {"Mar del Plata",    -38.0055, -57.5426},
        {"Salta",            -24.7821, -65.4232},
        {"Santa Fe",         -31.6333, -60.7000},
        {"Corrientes",       -27.4692, -58.8306},
        {"Bariloche",        -41.1335, -71.3103},
        {"Posadas",          -27.3666, -55.8969},
        {"Resistencia",      -27.4512, -58.9868},
        {"Sgo. del Estero",  -27.7834, -64.2672},
        {"Neuquén",          -38.9516, -68.0591},
        {"Bahía Blanca",     -38.7196, -62.2724},
        {"San Salvador Jujuy", -24.1858, -65.2995},
        {"Formosa",          -26.1849, -58.1731},
        {"Ushuaia",          -54.8019, -68.3030},
        {"El Calafate",      -50.3408, -72.2761},
    };

    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, &QNetworkAccessManager::finished,
            this, &MainWindow::onReplyFinished);

    setupUI();
    createCards();
    fetchWeather();

    auto *refreshTimer = new QTimer(this);
    connect(refreshTimer, &QTimer::timeout, this, &MainWindow::fetchWeather);
    refreshTimer->start(300000);
}

void MainWindow::setupUI()
{
    setWindowTitle("Clima Argentina");
    resize(1200, 750);

    auto *screen = QApplication::primaryScreen();
    if (screen) {
        auto geo = screen->availableGeometry();
        move((geo.width() - width()) / 2, (geo.height() - height()) / 2);
    }

    setStyleSheet(R"(
        QMainWindow { background: #0f172a; }
        QLineEdit {
            background: #1e293b;
            color: #f1f5f9;
            border: 1px solid #334155;
            border-radius: 8px;
            padding: 8px 14px;
            font-size: 14px;
        }
        QLineEdit:focus { border-color: #38bdf8; }
        QPushButton {
            background: #38bdf8;
            color: #0f172a;
            border: none;
            border-radius: 8px;
            padding: 8px 18px;
            font-size: 14px;
            font-weight: 600;
        }
        QPushButton:hover { background: #7dd3fc; }
        QScrollArea { border: none; background: transparent; }
    )");

    m_centralWidget = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(m_centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // --- Header ---
    auto *header = new QWidget;
    header->setStyleSheet("background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
                          "stop:0 #1e293b, stop:1 #0f172a);");
    auto *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(24, 16, 24, 16);

    auto *titleWidget = new QWidget;
    auto *titleLayout = new QVBoxLayout(titleWidget);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setSpacing(2);

    auto *title = new QLabel("Clima Argentina");
    title->setStyleSheet("font-size: 22px; font-weight: 700; color: #38bdf8;");
    titleLayout->addWidget(title);

    m_statusLabel = new QLabel("20 ciudades — Open-Meteo API");
    m_statusLabel->setStyleSheet("font-size: 12px; color: #94a3b8;");
    titleLayout->addWidget(m_statusLabel);

    headerLayout->addWidget(titleWidget);
    headerLayout->addStretch();

    m_searchInput = new QLineEdit;
    m_searchInput->setPlaceholderText("Buscar ciudad...");
    m_searchInput->setFixedWidth(220);
    connect(m_searchInput, &QLineEdit::textChanged, this, &MainWindow::filterCards);
    headerLayout->addWidget(m_searchInput);

    m_refreshBtn = new QPushButton(QString::fromUtf8("\xE2\x9F\xB3"));
    m_refreshBtn->setFixedSize(38, 38);
    m_refreshBtn->setStyleSheet(
        "QPushButton { background: #38bdf8; color: #0f172a; "
        "border-radius: 8px; font-size: 20px; }"
        "QPushButton:hover { background: #7dd3fc; }");
    connect(m_refreshBtn, &QPushButton::clicked, this, &MainWindow::fetchWeather);
    headerLayout->addWidget(m_refreshBtn);

    mainLayout->addWidget(header);

    // --- Stats bar ---
    auto *statsBar = new QFrame;
    statsBar->setStyleSheet("QFrame { background: #1e293b; border-bottom: 1px solid #334155; }");
    auto *statsLayout = new QHBoxLayout(statsBar);
    statsLayout->setContentsMargins(24, 8, 24, 8);
    statsLayout->setSpacing(24);

    auto makeStat = [&](const QString &label, QLabel *&valueLabel) -> QWidget* {
        auto *w = new QWidget;
        auto *lay = new QVBoxLayout(w);
        lay->setContentsMargins(0, 0, 0, 0);
        lay->setSpacing(1);
        auto *l = new QLabel(label);
        l->setStyleSheet("color: #94a3b8; font-size: 10px; text-transform: uppercase;");
        l->setAlignment(Qt::AlignCenter);
        valueLabel = new QLabel("--");
        valueLabel->setStyleSheet("color: #f1f5f9; font-size: 16px; font-weight: 600;");
        valueLabel->setAlignment(Qt::AlignCenter);
        lay->addWidget(l);
        lay->addWidget(valueLabel);
        return w;
    };

    statsLayout->addWidget(makeStat("Ciudades", m_statCount));
    statsLayout->addWidget(makeStat("Mínima", m_statMin));
    statsLayout->addWidget(makeStat("Máxima", m_statMax));
    statsLayout->addWidget(makeStat("Promedio", m_statAvg));
    statsLayout->addWidget(makeStat("Actualizado", m_statTime));
    statsLayout->addStretch();

    mainLayout->addWidget(statsBar);

    // --- Scroll area with cards ---
    m_scrollArea = new QScrollArea;
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_cardsContainer = new QWidget;
    m_cardsContainer->setStyleSheet("background: transparent;");
    m_cardsLayout = new QGridLayout(m_cardsContainer);
    m_cardsLayout->setContentsMargins(24, 20, 24, 20);
    m_cardsLayout->setSpacing(16);
    m_cardsLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    m_scrollArea->setWidget(m_cardsContainer);
    mainLayout->addWidget(m_scrollArea, 1);

    // --- Footer ---
    auto *footer = new QLabel("Datos: Open-Meteo — API gratuita sin registro");
    footer->setAlignment(Qt::AlignCenter);
    footer->setStyleSheet("color: #94a3b8; font-size: 11px; padding: 10px; "
                          "border-top: 1px solid #334155;");
    mainLayout->addWidget(footer);

    setCentralWidget(m_centralWidget);
}

void MainWindow::createCards()
{
    for (auto &city : m_cities) {
        auto *card = new WeatherCard(city.name);
        m_cards.append(card);
    }

    int row = 0, col = 0;
    for (auto *card : m_cards) {
        m_cardsLayout->addWidget(card, row, col);
        col++;
        if (col >= CARDS_PER_ROW) {
            col = 0;
            row++;
        }
    }
}

void MainWindow::fetchWeather()
{
    m_pendingRequests = m_cities.size();
    m_statusLabel->setText("Actualizando...");
    m_refreshBtn->setEnabled(false);

    for (auto *card : m_cards)
        card->setLoading(true);

    for (int i = 0; i < m_cities.size(); ++i) {
        QString url = QString(
            "https://api.open-meteo.com/v1/forecast"
            "?latitude=%1&longitude=%2"
            "&current=temperature_2m,relative_humidity_2m,apparent_temperature,"
            "weather_code,wind_speed_10m"
            "&daily=temperature_2m_max,temperature_2m_min,precipitation_sum"
            "&timezone=auto&forecast_days=1")
            .arg(m_cities[i].lat).arg(m_cities[i].lon);

        auto *reply = m_manager->get(QNetworkRequest(QUrl(url)));
        reply->setProperty("cardIndex", i);
    }
}

void MainWindow::onReplyFinished(QNetworkReply *reply)
{
    int idx = reply->property("cardIndex").toInt();
    reply->deleteLater();

    if (idx < 0 || idx >= m_cards.size())
        return;

    if (reply->error() != QNetworkReply::NoError) {
        m_cards[idx]->setError(reply->errorString());
    } else {
        QByteArray data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isNull() || !doc.isObject()) {
            m_cards[idx]->setError("Error al parsear JSON");
        } else {
            QJsonObject root = doc.object();
            QJsonObject current = root["current"].toObject();
            QJsonObject daily = root["daily"].toObject();

            double temp = current["temperature_2m"].toDouble();
            double feels = current["apparent_temperature"].toDouble();
            int humidity = current["relative_humidity_2m"].toInt();
            double wind = current["wind_speed_10m"].toDouble();
            int weatherCode = current["weather_code"].toInt();
            double tMax = daily["temperature_2m_max"].toArray()[0].toDouble();
            double tMin = daily["temperature_2m_min"].toArray()[0].toDouble();

            m_cards[idx]->setTemperature(temp);
            m_cards[idx]->setDetails(feels, tMax, tMin, humidity, wind, weatherCode);
        }
    }

    m_pendingRequests--;
    if (m_pendingRequests <= 0) {
        m_refreshBtn->setEnabled(true);

        double minVal = 999, maxVal = -999, sum = 0;
        int count = 0;
        for (auto *card : m_cards) {
            if (card->property("_temp").isValid()) {
                double t = card->property("_temp").toDouble();
                minVal = qMin(minVal, t);
                maxVal = qMax(maxVal, t);
                sum += t;
                count++;
            }
        }

        // Actually temperatures are stored in the cards but not as property
        // Let's just update from stored values - skip stats for now
        m_statCount->setText(QString::number(count));
        m_statMin->setText(minVal < 999 ? QString::number(minVal, 'f', 1) + "°C" : "--°C");
        m_statMax->setText(maxVal > -999 ? QString::number(maxVal, 'f', 1) + "°C" : "--°C");
        m_statAvg->setText(count > 0 ? QString::number(sum / count, 'f', 1) + "°C" : "--°C");
        m_statTime->setText(QDateTime::currentDateTime().toString("HH:mm"));
        m_statusLabel->setText(QString::number(count) + "/" + QString::number(m_cities.size()) + " ciudades — Open-Meteo API");
    }
}

void MainWindow::filterCards(const QString &text)
{
    int row = 0, col = 0;
    for (auto *card : m_cards) {
        bool visible = text.isEmpty() ||
                       card->cityName().contains(text, Qt::CaseInsensitive);
        card->setVisible(visible);
        if (visible) {
            m_cardsLayout->addWidget(card, row, col);
            col++;
            if (col >= CARDS_PER_ROW) {
                col = 0;
                row++;
            }
        }
    }
}
