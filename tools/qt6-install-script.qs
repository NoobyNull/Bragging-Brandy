// Qt6 Installation Script for 3D Model Management Utility
// This script automates the installation of Qt6 with required modules

function Controller() {
    installer.autoRejectMessageBoxes();
    installer.installationFinished.connect(function() {
        gui.clickButton(buttons.NextButton);
    });
}

Controller.prototype.WelcomePageCallback = function() {
    gui.clickButton(buttons.NextButton);
}

Controller.prototype.CredentialsPageCallback = function() {
    gui.clickButton(buttons.NextButton);
}

Controller.prototype.IntroductionPageCallback = function() {
    gui.clickButton(buttons.NextButton);
}

Controller.prototype.TargetDirectoryPageCallback = function() {
    gui.clickButton(buttons.NextButton);
}

Controller.prototype.ComponentSelectionPageCallback = function() {
    var widget = gui.currentPageWidget();

    // Select Qt 6.2.0
    widget.selectComponent("qt.qt6.620");

    // Select MSVC 2019 64-bit
    widget.selectComponent("qt.qt6.620.win64_msvc2019_64");

    // Select required modules
    widget.selectComponent("qt.qt6.620.doc");
    widget.selectComponent("qt.qt6.620.examples");

    // Core modules (always installed)
    // qt.qt6.620.core

    // Required modules for our application
    widget.selectComponent("qt.qt6.620.widgets");
    widget.selectComponent("qt.qt6.620.opengl");
    widget.selectComponent("qt.qt6.620.sql");
    widget.selectComponent("qt.qt6.620.charts");

    // Development tools
    widget.selectComponent("qt.qt6.620.debug_info");
    widget.selectComponent("qt.qt6.620.dev");

    gui.clickButton(buttons.NextButton);
}

Controller.prototype.LicenseAgreementPageCallback = function() {
    gui.currentPageWidget().AcceptLicenseRadioButton.setChecked(true);
    gui.clickButton(buttons.NextButton);
}

Controller.prototype.StartMenuDirectoryPageCallback = function() {
    gui.clickButton(buttons.NextButton);
}

Controller.prototype.ReadyForInstallationPageCallback = function() {
    gui.clickButton(buttons.NextButton);
}

Controller.prototype.FinishedPageCallback = function() {
    var checkBoxForm = gui.currentPageWidget().LaunchQtCreatorCheckBoxForm;
    if (checkBoxForm && checkBoxForm.launchQtCreatorCheckBox) {
        checkBoxForm.launchQtCreatorCheckBox.checked = false;
    }
    gui.clickButton(buttons.FinishButton);
}