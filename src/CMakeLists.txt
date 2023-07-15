#
# Copyright 2020 Han Young <hanyoung@protonmail.com>
# Copyright 2020-2021 Devin Lin <devin@kde.org>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#

set(kclock_SRCS
)

qt_add_resources(RESOURCES resources.qrc)

add_executable(kclock main.cpp ${kclock_SRCS} ${RESOURCES})

target_link_libraries(kclock
	Qt::Widgets
	Qt::Qml
        Qt::Gui
        Qt::QuickControls2
        KF${QT_MAJOR_VERSION}::I18n
        )
target_include_directories(kclock PRIVATE ${CMAKE_BINARY_DIR})
install(TARGETS kclock ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
