#include "weathercard.h"
#include <QFont>
#include <QGraphicsDropShadowEffect>

WeatherCard::WeatherCard(const QString &cityName, QWidget *parent)
    : QFrame(parent), m_cityName(cityName)
{
    setFixedSize(220, 180);
    setObjectName("WeatherCard");

    auto *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setOffset(0, 4);
    shadow->setColor(QColor(0, 0, 0, 80));
    setGraphicsEffect(shadow);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(14, 10, 14, 10);
    layout->setSpacing(4);

    m_nameLabel = new QLabel(cityName, this);
    m_nameLabel->setObjectName("cardName");
    m_nameLabel->setWordWrap(true);

    m_tempLabel = new QLabel("--°C", this);
    m_tempLabel->setObjectName("cardTemp");

    m_feelsLabel = new QLabel(this);
    m_feelsLabel->setObjectName("cardFeels");

    m_detailsWidget = new QWidget(this);
    auto *detLayout = new QVBoxLayout(m_detailsWidget);
    detLayout->setContentsMargins(0, 0, 0, 0);
    detLayout->setSpacing(2);

    m_maxLabel = new QLabel(this);
    m_minLabel = new QLabel(this);
    m_humidityLabel = new QLabel(this);
    m_windLabel = new QLabel(this);

    for (auto *l : {m_maxLabel, m_minLabel, m_humidityLabel, m_windLabel})
        l->setObjectName("cardDetail");

    detLayout->addWidget(m_maxLabel);
    detLayout->addWidget(m_minLabel);
    detLayout->addWidget(m_humidityLabel);
    detLayout->addWidget(m_windLabel);

    m_errorLabel = new QLabel(this);
    m_errorLabel->setObjectName("cardError");
    m_errorLabel->hide();

    layout->addWidget(m_nameLabel);
    layout->addWidget(m_tempLabel);
    layout->addWidget(m_feelsLabel);
    layout->addWidget(m_detailsWidget);
    layout->addWidget(m_errorLabel);
    layout->addStretch();

    setStyleSheet(R"(
        #WeatherCard {
            background: #1e293b;
            border-radius: 12px;
            border: none;
        }
        #cardName {
            color: #f1f5f9;
            font-size: 13px;
            font-weight: 600;
        }
        #cardTemp {
            color: #f1f5f9;
            font-size: 28px;
            font-weight: 700;
        }
        #cardFeels {
            color: #94a3b8;
            font-size: 11px;
        }
        #cardDetail {
            color: #94a3b8;
            font-size: 11px;
        }
        #cardError {
            color: #f87171;
            font-size: 12px;
        }
    )");

    setLoading(true);
}

void WeatherCard::setTemperature(double temp)
{
    m_tempLabel->setText(QString::number(temp, 'f', 1) + "°C");
    applyTempStyle(temp);
}

void WeatherCard::setDetails(double feelsLike, double max, double min,
                              int humidity, double wind, int weatherCode)
{
    m_hasData = true;
    m_feelsLabel->setText("Sensación: " + QString::number(feelsLike, 'f', 1) + "°C");
    m_maxLabel->setText(QString::fromUtf8("\xF0\x9F\x93\x88 Máx: ") + QString::number(max, 'f', 1) + "°C");
    m_minLabel->setText(QString::fromUtf8("\xF0\x9F\x93\x89 Mín: ") + QString::number(min, 'f', 1) + "°C");
    m_humidityLabel->setText(QString::fromUtf8("\xF0\x9F\x92\xA7 Humedad: ") + QString::number(humidity) + "%");
    m_windLabel->setText(QString::fromUtf8("\xF0\x9F\x92\xA8 Viento: ") + QString::number(wind, 'f', 0) + " km/h");
    m_nameLabel->setText(weatherEmoji(weatherCode) + " " + m_cityName);
    m_errorLabel->hide();
    m_detailsWidget->show();
}

void WeatherCard::setError(const QString &msg)
{
    m_hasData = false;
    m_tempLabel->setText("--°C");
    m_tempLabel->setStyleSheet("color: #f1f5f9; font-size: 28px; font-weight: 700;");
    m_feelsLabel->clear();
    m_detailsWidget->hide();
    m_errorLabel->setText(QString::fromUtf8("\xE2\x9A\xA0\xEF\xB8\x8F ") + msg);
    m_errorLabel->show();
    setStyleSheet(R"(
        #WeatherCard {
            background: #1e293b;
            border-radius: 12px;
            border: none;
        }
        #cardName { color: #f1f5f9; font-size: 13px; font-weight: 600; }
        #cardTemp { color: #f1f5f9; font-size: 28px; font-weight: 700; }
        #cardFeels { color: #94a3b8; font-size: 11px; }
        #cardDetail { color: #94a3b8; font-size: 11px; }
        #cardError { color: #f87171; font-size: 12px; }
    )");
}

void WeatherCard::setLoading(bool loading)
{
    if (loading) {
        m_tempLabel->setText("...");
        m_feelsLabel->clear();
        m_detailsWidget->hide();
        m_errorLabel->hide();
    }
}

void WeatherCard::applyTempStyle(double temp)
{
    QString borderColor;
    QString bgColor = "#1e293b";

    if (temp <= 0)       borderColor = "#818cf8";
    else if (temp <= 10) borderColor = "#38bdf8";
    else if (temp <= 20) borderColor = "#4ade80";
    else if (temp <= 30) borderColor = "#fbbf24";
    else if (temp <= 40) borderColor = "#f97316";
    else                 borderColor = "#ef4444";

    setStyleSheet(QString(R"(
        #WeatherCard {
            background: %1;
            border-radius: 12px;
            border-top: 4px solid %2;
            border-left: none;
            border-right: none;
            border-bottom: none;
        }
        #cardName { color: #f1f5f9; font-size: 13px; font-weight: 600; }
        #cardTemp { color: #f1f5f9; font-size: 28px; font-weight: 700; }
        #cardFeels { color: #94a3b8; font-size: 11px; }
        #cardDetail { color: #94a3b8; font-size: 11px; }
        #cardError { color: #f87171; font-size: 12px; }
    )").arg(bgColor, borderColor));
}

QString WeatherCard::weatherEmoji(int code)
{
    if (code == 0) return QString::fromUtf8("\xE2\x98\x80\xEF\xB8\x8F");
    if (code <= 3) return QString::fromUtf8("\xE2\x9B\x85");
    if (code <= 48) return QString::fromUtf8("\xF0\x9F\x8C\xAB\xEF\xB8\x8F");
    if (code <= 57) return QString::fromUtf8("\xF0\x9F\x8C\xA6\xEF\xB8\x8F");
    if (code <= 67) return QString::fromUtf8("\xF0\x9F\x8C\xA7\xEF\xB8\x8F");
    if (code <= 77) return QString::fromUtf8("\xF0\x9F\x8C\xA8\xEF\xB8\x8F");
    if (code <= 82) return QString::fromUtf8("\xF0\x9F\x8C\xA6\xEF\xB8\x8F");
    if (code <= 86) return QString::fromUtf8("\xF0\x9F\x8C\xA8\xEF\xB8\x8F");
    return QString::fromUtf8("\xE2\x98\x81\xEF\xB8\x8F");
}
