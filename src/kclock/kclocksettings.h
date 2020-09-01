#pragma once

#include <QObject>
#include "kclocksettingsinterface.h"
class KClockSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(QString alarmSilenceAfterDisplay READ alarmSilenceAfterDisplay WRITE setAlarmSilenceAfterDisplay NOTIFY alarmSilenceChanged)
    Q_PROPERTY(QString alarmSnoozeLengthDisplay READ alarmSnoozeLengthDisplay WRITE setAlarmSnoozeLengthDisplay NOTIFY alarmSnoozedChanged)
    Q_PROPERTY(int alarmSilenceAfter READ alarmSilenceAfter WRITE setAlarmSilenceAfter NOTIFY alarmSilenceChanged)
    Q_PROPERTY(int alarmSnoozeLength READ alarmSnoozeLength WRITE setAlarmSnoozeLength NOTIFY alarmSnoozedChanged)
public:
    static KClockSettings& instance(){
        static KClockSettings singleton;
        return singleton;
    };

    const int & volume() const
    {
        return m_volume;
    }

    void setVolume(int volume){
        m_interface->setProperty("alarmVolume", volume);
    }

    const QString &alarmSilenceAfterDisplay() const{
        return m_alarmSilenceAfterDisplay;
    }

    void setAlarmSilenceAfterDisplay(QString str){
        m_alarmSilenceAfterDisplay = str;
    }

    const QString &alarmSnoozeLengthDisplay() const{
        return m_alarmSnoozeLengthDisplay;
    }

    void setAlarmSnoozeLengthDisplay(QString str){
        m_alarmSnoozeLengthDisplay = str;
    }

    const int& alarmSilenceAfter() const{
        return m_alarmSilenceAfter;
    }

    void setAlarmSilenceAfter(int length){
        m_interface->setProperty("alarmSilenceAfter", length);
    }

    void setAlarmSnoozeLength(int length){
        m_interface->setProperty("alarmSnoozeLength", length);
    }

    const int& alarmSnoozeLength() const{
        return m_alarmSnoozeLength;
    }
signals:
    void volumeChanged();
    void alarmSilenceChanged();
    void alarmSnoozedChanged();
private slots:
    void updateVolume();
    void updateAlarmSilenceAfter();
    void updateAlarmSnoozeLength();
private:
    KClockSettings();
    LocalKClockSettingsInterface * m_interface;

    QString m_alarmSilenceAfterDisplay;
    QString m_alarmSnoozeLengthDisplay;

    int m_volume;
    int m_alarmSilenceAfter;
    int m_alarmSnoozeLength;
};
