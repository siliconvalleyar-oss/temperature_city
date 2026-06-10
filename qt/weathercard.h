#ifndef WEATHERCARD_H
#define WEATHERCARD_H

#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>
#include <QString>

class WeatherCard : public QFrame {
    Q_OBJECT
public:
    explicit WeatherCard(const QString &cityName, QWidget *parent = nullptr);

    void setTemperature(double temp);
    void setDetails(double feelsLike, double max, double min,
                    int humidity, double wind, int weatherCode);
    void setError(const QString &msg);
    void setLoading(bool loading);
    QString cityName() const { return m_cityName; }

private:
    void applyTempStyle(double temp);
    static QString weatherEmoji(int code);

    QString m_cityName;
    QLabel *m_nameLabel;
    QLabel *m_tempLabel;
    QLabel *m_feelsLabel;
    QLabel *m_maxLabel;
    QLabel *m_minLabel;
    QLabel *m_humidityLabel;
    QLabel *m_windLabel;
    QLabel *m_errorLabel;
    QWidget *m_detailsWidget;
    bool m_hasData = false;
};

#endif
