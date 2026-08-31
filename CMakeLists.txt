cmake_minimum_required(VERSION 3.16)

project(UDH_Universite VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Charts LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Charts LinguistTools)

set(TS_FILES UDH_Universite_fr_CA.ts)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        etudiant.cpp
        etudiant.h
        studentauthwindow.cpp
        studentauthwindow.h
        studentdashboardwindow.cpp
        studentdashboardwindow.h
        registrationdialog.cpp
        registrationdialog.h
        landingwindow.cpp
        landingwindow.h
        teacherauthwindow.cpp
        teacherauthwindow.h
        adminauthwindow.cpp
        adminauthwindow.h
        professeur.cpp
        professeur.h
        teacherdashboardwindow.cpp
        teacherdashboardwindow.h
        administrateur.cpp
        administrateur.h
        admindashboardwindow.cpp
        admindashboardwindow.h
        gestionsalaires.cpp
        gestionsalaires.h
        tresorerie.cpp
        tresorerie.h
        statutsalaireprofesseur.cpp
        statutsalaireprofesseur.h
        fichedetailinscription.cpp
        fichedetailinscription.h
        validationinscriptions.cpp
        validationinscriptions.h
        annonce.cpp
        annonce.h
        gestionannonces.cpp
        gestionannonces.h
        annoncesrecues.cpp
        annoncesrecues.h
        modulesupport.cpp
        modulesupport.h
        calendrierscolaire.cpp
        calendrierscolaire.h
        gestionreunions.cpp
        gestionreunions.h
        gestioncourrier.cpp
        gestioncourrier.h
        paiementversement.cpp
        paiementversement.h
        modulerapportgraphique.cpp
        modulerapportgraphique.h
        modulesupervision.cpp
        modulesupervision.h
        dossieradministratifnavbar.cpp
        dossieradministratifnavbar.h
        evenementsadminwidget.cpp
        evenementsadminwidget.h
        modulegestionmembres.cpp
        modulegestionmembres.h
        modulevueensemble.cpp
        modulevueensemble.h
        modulevalidationacademique.cpp
        modulevalidationacademique.h
        modulegestionfacultes.cpp
        modulegestionfacultes.h
        isolationfaculteutils.cpp
        isolationfaculteutils.h
        parametrescomptebase.cpp
        parametrescomptebase.h
        resources.qrc
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(UDH_Universite
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(UDH_Universite SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(UDH_Universite
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(UDH_Universite PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Charts)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.UDH_Universite)
endif()
set_target_properties(UDH_Universite PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS UDH_Universite
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(UDH_Universite)
endif()
