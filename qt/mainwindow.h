#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QTimer>
#include <QVector>

#include "weathercard.h"

struct CityData {
    QString name;
    double lat;
    double lon;
};

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void fetchWeather();
    void onReplyFinished(QNetworkReply *reply);
    void filterCards(const QString &text);

private:
    void setupUI();
    void setupStats();
    void createCards();

    QNetworkAccessManager *m_manager;
    QWidget *m_centralWidget;
    QWidget *m_cardsContainer;
    QGridLayout *m_cardsLayout;
    QScrollArea *m_scrollArea;
    QLineEdit *m_searchInput;
    QPushButton *m_refreshBtn;
    QLabel *m_statCount;
    QLabel *m_statMin;
    QLabel *m_statMax;
    QLabel *m_statAvg;
    QLabel *m_statTime;
    QLabel *m_statusLabel;

    QVector<CityData> m_cities;
    QVector<WeatherCard *> m_cards;
    int m_pendingRequests = 0;
};

#endif
