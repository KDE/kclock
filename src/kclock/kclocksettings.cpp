#include "kclocksettings.h"
#include "alarmplayer.h"
#include <KLocalizedString>
#include <QDebug>
KClockSettings::KClockSettings()
    : m_interface(new LocalKClockSettingsInterface(QStringLiteral("org.kde.kclockd"), QStringLiteral("/Settings"), QDBusConnection::sessionBus()))
{
    m_volume = m_interface->alarmVolume();
    AlarmPlayer::instance().setVolume(m_volume);

    m_alarmSilenceAfter = m_interface->alarmSilenceAfter();
    m_alarmSnoozeLength = m_interface->alarmSnoozeLength();

    connect(m_interface, &LocalKClockSettingsInterface::alarmVolumeChanged, this, &KClockSettings::updateVolume);
    connect(m_interface, &LocalKClockSettingsInterface::alarmSilenceAfterChanged, this, &KClockSettings::updateAlarmSilenceAfter);
    connect(m_interface, &LocalKClockSettingsInterface::alarmSnoozeLengthChanged, this, &KClockSettings::updateAlarmSnoozeLength);

    // init display
    switch (m_alarmSilenceAfter) {
    case 30:
        m_alarmSilenceAfterDisplay = i18n("30 seconds");
        break;
    case 60:
        m_alarmSilenceAfterDisplay = i18n("1 minute");
        break;
    case 300:
        m_alarmSilenceAfterDisplay = i18n("5 minutes");
        break;
    case 600:
        m_alarmSilenceAfterDisplay = i18n("10 minutes");
        break;
    case 900:
        m_alarmSilenceAfterDisplay = i18n("15 minutes");
        break;
    case -1:
        m_alarmSilenceAfterDisplay = i18n("Never");
        break;
    }

    switch (m_alarmSnoozeLength) {
    case 1:
        m_alarmSnoozeLengthDisplay = i18n("1 minute");
        break;
    case 2:
        m_alarmSnoozeLengthDisplay = i18n("2 minutes");
        break;
    case 3:
        m_alarmSnoozeLengthDisplay = i18n("3 minutes");
        break;
    case 4:
        m_alarmSnoozeLengthDisplay = i18n("4 minutes");
        break;
    case 5:
        m_alarmSnoozeLengthDisplay = i18n("5 minutes");
        break;
    case 10:
        m_alarmSnoozeLengthDisplay = i18n("10 minutes");
        break;
    case 30:
        m_alarmSnoozeLengthDisplay = i18n("30 minutes");
        break;
    case 60:
        m_alarmSnoozeLengthDisplay = i18n("1 hour");
        break;
    }
    qDebug() << m_alarmSnoozeLengthDisplay;
}

void KClockSettings::updateVolume()
{
    m_volume = m_interface->alarmVolume();
    Q_EMIT volumeChanged();
}

void KClockSettings::updateAlarmSilenceAfter()
{
    m_alarmSilenceAfter = m_interface->alarmSilenceAfter();
    Q_EMIT alarmSilenceChanged();
}

void KClockSettings::updateAlarmSnoozeLength()
{
    m_alarmSnoozeLength = m_interface->alarmSnoozeLength();
    Q_EMIT alarmSnoozedChanged();
}
