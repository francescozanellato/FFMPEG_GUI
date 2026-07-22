#include "universal_gui.h"
#include <QFormLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QSettings>
#include <QCoreApplication>
#include <QMessageBox>
#include <QProcess>
#include <QFileInfo>
#include <QScreen>
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QtGlobal>
//#include <QStyle>

DropLineEdit::DropLineEdit(QWidget *parent) : QLineEdit(parent) {
    setAcceptDrops(true); // Abilita il drop sul widget
}

void DropLineEdit::dragEnterEvent(QDragEnterEvent *e) {
    if (e->mimeData()->hasUrls()) e->acceptProposedAction();
}

void DropLineEdit::dropEvent(QDropEvent *e) {
    if (e->mimeData()->hasUrls()) {
        setText(e->mimeData()->urls().first().toLocalFile());
        emit fileDropped(); // <--- EMETTE IL SEGNALE QUI
    }
}

UniversalGUI::UniversalGUI(QWidget *parent) : QWidget(parent) {
    auto *layout = new QFormLayout(this);

    thisAppName = QFileInfo(QCoreApplication::applicationFilePath()).baseName();
    this->setWindowTitle(thisAppName+ " v" APP_VERSION);

    // Lambda helper per creare riga con DropLineEdit + Bottone
    auto createRow = [this](DropLineEdit*& edit, QPushButton* btn, auto slot) {
        edit = new DropLineEdit(this);


        edit->setClearButtonEnabled(true);

        connect(edit, &DropLineEdit::fileDropped, this, &UniversalGUI::fill_defaultEditOutputField);

        auto *h = new QHBoxLayout;
        h->addWidget(edit);
        h->addWidget(btn);
        connect(btn, &QPushButton::clicked, this, slot);
        return h;
    };

    layout->addRow(tr("File Input 1 (%1):"), createRow(editVideo, new QPushButton(tr("Browse...")), &UniversalGUI::browseVideo));
    layout->addRow(tr("File Input 2 (%2):"), createRow(editAudio1, new QPushButton("Browse..."), &UniversalGUI::browseAudio1));
    layout->addRow(tr("File Input 3 (%3):"), createRow(editAudio2, new QPushButton("Browse..."), &UniversalGUI::browseAudio2));
    layout->addRow(tr("File Input 4 (%4):"), createRow(editAudio3, new QPushButton("Browse..."), &UniversalGUI::browseAudio3));
    layout->addRow(tr("Output File (%5):"), createRow(editOutput, new QPushButton("Browse..."), &UniversalGUI::browseOutput));

    comboOptions = new QComboBox(this);
    comboOptions->setEditable(true);
    layout->addRow(tr("Program Options:"), comboOptions);

    comboCombinations = new QComboBox(this);
    layout->addRow(tr("Shortcuts:"), comboCombinations);

    btnRun = new QPushButton(tr("Run Command"), this);
    btnRun->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton)); //QStyle::SP_DialogOpenButton
    btnStop = new QPushButton(tr("Stop Command"), this);
    btnStop->setIcon(style()->standardIcon(QStyle::SP_DialogCancelButton)); //QStyle::SP_DialogCancelButton
    btnStop->setEnabled(false); // Disabilitato all'avvio

    btnSettings = new QPushButton(tr("Open Settings"), this);
    btnSettings->setIcon(style()->standardIcon(QStyle::SP_FileDialogDetailedView)); //QStyle::SP_DialogOpenButton
    btnLicense = new QPushButton(tr("License"), this);
    btnLicense->setIcon(style()->standardIcon(QStyle::SP_MessageBoxWarning)); //QStyle::SP_DialogOpenButton
    btnAboutQt = new QPushButton(tr("About Qt"), this);
    btnAboutQt->setIcon(style()->standardIcon(QStyle::SP_TitleBarMenuButton));
    //layout->addRow(btnRun);
    // Disposizione affiancata
    auto *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(btnRun);
    btnLayout->addWidget(btnStop);
    btnLayout->addStretch();
    btnLayout->addWidget(btnSettings);
    btnLayout->addWidget(btnLicense);
    btnLayout->addWidget(btnAboutQt);
    layout->addRow("Actions:", btnLayout);


    // Inizializzazione QProcess
    programProcess = new QProcess(this);
    programProcess->setProcessChannelMode(QProcess::MergedChannels); // Unifica stdout e stderr

    // Creazione UI Console
    consoleOutput = new QPlainTextEdit(this);
    consoleOutput->setReadOnly(true);
    consoleOutput->setFont(QFont("Consolas", 9)); // Font monospazio tipico delle console

    consoleInput = new QLineEdit(this);
    consoleInput->setPlaceholderText(tr("Write here to interact (e.g. 'y' to overwrite) and press Enter..."));
    consoleInput->setClearButtonEnabled(true);

    layout->addRow(tr("Console:"), consoleOutput);
    layout->addRow(tr("Input:"), consoleInput);

    // Connessioni per il processo e la console
    connect(programProcess, &QProcess::readyReadStandardOutput, this, &UniversalGUI::readProcessOutput);
    connect(consoleInput, &QLineEdit::returnPressed, this, &UniversalGUI::sendConsoleInput);

    // Gestione compatibile con Qt5 e Qt6 per il segnale finished
    connect(programProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &UniversalGUI::processFinished);


    // Quando si seleziona una combinazione, aggiorna il campo modificabile
    connect(comboCombinations, &QComboBox::currentTextChanged, [this]() {
        comboOptions->setCurrentText(comboCombinations->currentData().toString());
    });
    
    connect(btnRun, &QPushButton::clicked, this, &UniversalGUI::executeCommand);
    connect(btnStop, &QPushButton::clicked, this, &UniversalGUI::stopProcess);
    connect(btnSettings, &QPushButton::clicked, this, &UniversalGUI::openSettingsFile);
    connect(btnLicense, &QPushButton::clicked, this, &UniversalGUI::showLicense);
    connect(btnAboutQt, &QPushButton::clicked, this, &UniversalGUI::aboutQt);

    loadSettings();
    editVideo->setFocus();

    const QStringList args = QCoreApplication::arguments();

    if (args.size() >= 2) { editVideo->setText(args.at(1));
        //*editAudio1, *editAudio2, *editAudio3, *editOutput;
        fill_defaultEditOutputField();
    }
    if (args.size() >= 3) editAudio1->setText(args.at(2));
    if (args.size() >= 4) editAudio2->setText(args.at(3));
    if (args.size() >= 5) editAudio3->setText(args.at(4));
    if (args.size() >= 6) editOutput->setText(args.at(5));


    // Esegue la funzione ogni volta che il testo del campo video cambia
    //connect(editVideo, &QLineEdit::textChanged, this, &UniversalGUI::fill_defaultEditOutputField);
}

void UniversalGUI::aboutQt() {
    QMessageBox::aboutQt(this);
}

void UniversalGUI::showLicense() {
    //QMessageBox::information(this,"License", QString(MY_COPYRIGHT).replace("_"," "));
    // ... all'interno di una funzione membro (es. MainWindow)
    QString copyrightText = QString::fromUtf8(MY_COPYRIGHT).replace("_"," ");

    // Usiamo &copy; per il simbolo © in HTML e tr() per l'intero blocco di testo
    QString disclaimer = tr(
                "<h3>License Information</h3>"
                "<p>&copy; %1</p>"
                "<p>This program is free software: you can redistribute it and/or modify "
                "it under the terms of the GNU General Public License as published by "
                "the Free Software Foundation, either version 3 of the License, or "
                "(at your option) any later version.</p>"
                "<p><i>This program is distributed in the hope that it will be useful, "
                "but WITHOUT ANY WARRANTY; without even the implied warranty of "
                "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.</i></p>"
                ).arg(copyrightText);

    QMessageBox::about(this, tr("License"), disclaimer);
}

void UniversalGUI::openSettingsFile() {
    QString iniPath = QCoreApplication::applicationDirPath() + "/" + thisAppName +".ini";
    if (!QFile::exists(iniPath)) {
        QMessageBox::warning(this, tr("Warning"), tr("Configuration file is not present. Close and open again this program to generate a default one"));
        return;
    }
    // Apre il file con l'editor di testo predefinito di Windows/Linux/macOS
    QDesktopServices::openUrl(QUrl::fromLocalFile(iniPath));
}

QString UniversalGUI::browseFile(bool save) {
    return save ? QFileDialog::getSaveFileName(this, tr("Salva Output File"))
                : QFileDialog::getOpenFileName(this, tr("Select Input File"));
}

void UniversalGUI::browseVideo() {
    QString file = browseFile();
    if (!file.isEmpty()) {
        editVideo->setText(file);
        fill_defaultEditOutputField(); // <--- Chiamata esplicita qui
    }
}

void UniversalGUI::browseAudio1() { editAudio1->setText(browseFile()); }
void UniversalGUI::browseAudio2() { editAudio2->setText(browseFile()); }
void UniversalGUI::browseAudio3() { editAudio3->setText(browseFile()); }
void UniversalGUI::browseOutput() { editOutput->setText(browseFile(true)); }

void UniversalGUI::loadSettings() {
    QString iniPath = QCoreApplication::applicationDirPath() + "/" + thisAppName +".ini";
    QSettings settings(iniPath, QSettings::IniFormat);

#ifdef Q_OS_WINDOWS
    programPath = settings.value("Program/program_path", "C:/PortableApps/ffmpeg/ffmpeg/bin/ffmpeg.exe").toString();
#elif defined(Q_OS_LINUX)
    programPath = settings.value("Program/program_path", "ffmpeg").toString();
#else
    // Codice per altri sistemi operativi (es. Q_OS_MACOS per macOS)
    programPath = settings.value("Program/program_path", "ffmpeg").toString();
#endif


    // --- RIPRISTINO GEOMETRIA E STATO NATIVO ---
    if (settings.contains("Window/geometry")) {
        this->restoreGeometry(settings.value("Window/geometry").toByteArray());
    } else {
        // Fallback di default al primissimo avvio assoluto
        this->resize(1024, 200);
    }

    int consoleMinH = settings.value("Window/console_min_height", 400).toInt();
    consoleOutput->setMinimumHeight(consoleMinH);
    // --------------------------------------------

    if (settings.contains("Options/opt1")) {
        settings.beginGroup("Options");
        for (const QString &key : settings.childKeys()) {
            comboOptions->addItem(settings.value(key).toString());
        }
        settings.endGroup();
    } else {
        comboOptions->addItem("-i %1 -i %2 -c:v copy -c:a aac %5");
        comboOptions->addItem("-i %1 -i %2 -i %3 -filter_complex amix=inputs=2 %5");
    }

    settings.beginGroup("Combinations");
    for (const QString &key : settings.childKeys()) {
        comboCombinations->addItem(key, settings.value(key).toString());
    }
    if (comboCombinations->count() > 0) {
        comboOptions->setCurrentText(comboCombinations->currentData().toString());
    } else {
        comboCombinations->addItem("MuxVideoWithAudio1", "-i %1 -i %2 -c:v copy -c:a aac -map 0:v:0 -map 1:a:0 %5");
        comboCombinations->addItem("ExtractAudioFromVideo", "-i %1 -vn -acodec libmp3lame %5");
        comboCombinations->addItem("MixAllAudio", "-i %2 -i %3 -i %4 -filter_complex amix=inputs=3:duration=first %5");
        comboOptions->setCurrentText(comboCombinations->currentData().toString());
    }

    settings.endGroup();




    // Mantiene a schermo e in memoria solo le ultime 1000 righe di output
    consoleOutput->setMaximumBlockCount(settings.value("Window/console_max_history_rows", 1000).toInt());
}

void UniversalGUI::closeEvent(QCloseEvent *event) {
    //INCLUDES savesettings command
    QString iniPath = QCoreApplication::applicationDirPath() + "/" + thisAppName +".ini";
    QSettings settings(iniPath, QSettings::IniFormat);

    settings.setValue("Program/program_path", programPath);

    // Salva in automatico posizione, dimensioni e stato (anche se massimizzato)
    settings.setValue("Window/width", this->width());
    settings.setValue("Window/height", this->height());
    settings.setValue("Window/geometry", this->saveGeometry());
    // Salva l'altezza attuale della console nel caso l'utente l'abbia modificata trascinando il divisore
    settings.setValue("Window/console_min_height", consoleOutput->height());
    settings.setValue("Window/console_max_history_rows", consoleOutput->maximumBlockCount());

    settings.beginGroup("Options");
    if (comboOptions->count() > 0) {
        for (int ii = 0; ii < comboOptions->count(); ii++) {
            settings.setValue("opt"+QString::number(ii+1), comboOptions->itemText(ii));
        }
    } else {
        settings.setValue("opt1", "-i %1 -i %2 -c:v copy -c:a aac %5");
        settings.setValue("opt2", "-i %1 -i %2 -i %3 -filter_complex amix=inputs=2 %5");
    }
    settings.endGroup();

    settings.beginGroup("Combinations");
    if (comboCombinations->count() > 0) {
        for (int ii = 0; ii< comboCombinations->count(); ii++) {
            settings.setValue(comboCombinations->itemText(ii), comboCombinations->itemData(ii).toString());
        }
    } else {
        settings.setValue("MuxVideoWithAudio1", "-i %1 -i %2 -c:v copy -c:a aac -map 0:v:0 -map 1:a:0 %5");
        settings.setValue("ExtractAudioFromVideo", "-i %1 -vn -acodec libmp3lame %5");
        settings.setValue("MixAllAudio", "-i %2 -i %3 -i %4 -filter_complex amix=inputs=3:duration=first %5");
    }
    settings.endGroup();

    if (comboCombinations->count() > 0) {
        comboOptions->setCurrentText(comboCombinations->currentData().toString());
    }

    QWidget::closeEvent(event); // Accetta l'evento e chiude la finestra
}

void UniversalGUI::fill_defaultEditOutputField() {
    if (editOutput->text().isEmpty()) {
        QFileInfo fi(editVideo->text());
        // Estrae l'estensione (es. "mp4"). Se esiste, aggiunge il punto.
        QString ext = fi.completeSuffix().isEmpty() ? "" : "." + fi.completeSuffix();

        // Ricostruisce il percorso: Cartella + NomeSenzaEstensione + __converted + .Estensione
        QString newPath = fi.absolutePath() + "/" + fi.completeBaseName() + "__converted" + ext;
        editOutput->setText(newPath);
    }
}

void UniversalGUI::readProcessOutput() {
    // Legge l'output del programma e lo aggiunge alla casella di testo
    QByteArray out = programProcess->readAllStandardOutput();

    // Sposta il cursore alla fine per lo scrolling automatico
    consoleOutput->moveCursor(QTextCursor::End);
    consoleOutput->insertPlainText(QString::fromLocal8Bit(out));
    consoleOutput->moveCursor(QTextCursor::End);
}

void UniversalGUI::sendConsoleInput() {
    // Se il processo è in esecuzione, invia il testo + Ritorno a capo
    if (programProcess->state() == QProcess::Running) {
        QString input = consoleInput->text() + "\n";
        programProcess->write(input.toUtf8());
        consoleInput->clear();
    }
}

void UniversalGUI::processFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    QString status = (exitStatus == QProcess::NormalExit) ? "completato" : "andato in crash";
    consoleOutput->appendPlainText(QString(tr("\n--- PROCESS FINISHED (Codes: %1, %2) ---")).arg(exitCode).arg(status));
    btnRun->setEnabled(true);   // Riabilita il tasto avvia
    btnStop->setEnabled(false);  // Disabilita il tasto stop
}

void UniversalGUI::stopProcess() {
    if (programProcess->state() == QProcess::Running) {
        consoleOutput->appendPlainText(tr("\n--- PROCESS KILLED ---"));
        programProcess->kill(); // Chiude istantaneamente il programma a riga di comando
    }
}

void UniversalGUI::executeCommand() {

    if (editVideo->text().isEmpty()) {
        QMessageBox::information(this, "Warning", "\"Video/Audio Input\" field cannot be empty. Exiting.");
        return;
    } else {
        fill_defaultEditOutputField();
    }
    QString cmd = comboOptions->currentText();
    
    // Sostituzione segnaposti. Aggiunge i doppi apici per gestire percorsi con spazi.
    cmd.replace("%1", QString("\"%1\"").arg(editVideo->text()))
            .replace("%2", QString("\"%1\"").arg(editAudio1->text()))
            .replace("%3", QString("\"%1\"").arg(editAudio2->text()))
            .replace("%4", QString("\"%1\"").arg(editAudio3->text()))
            .replace("%5", QString("\"%1\"").arg(editOutput->text()));

    QString finalCommand = QString("\"%1\" %2").arg(programPath, cmd);
    
    // Finestra di debug per mostrare il comando generato
    QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Run Confirmation"),
                                                              QString(tr("Do you want to run the following command?\n\n%1")).arg(finalCommand),
                                                              QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No) {
        return; // Si ferma e non procede
    }

    // Esecuzione reale con QProcess usando il path configurato
    //QProcess::startDetached(programPath, QProcess::splitCommand(cmd));

    // Esecuzione interattiva della console:
    if (programProcess->state() == QProcess::Running) {
        QMessageBox::warning(this, tr("Warning"), tr("A process is already running!"));
        return;
    }

    consoleInput->setFocus();

    consoleOutput->clear();
    consoleOutput->appendPlainText(tr("Running command:\n") + finalCommand + "\n");

    // Separa gli argomenti in modo corretto per QProcess
    QStringList args = QProcess::splitCommand(cmd);
    programProcess->start(programPath, args);
    btnRun->setEnabled(false);  // Disabilita il tasto avvia durante l'elaborazione
    btnStop->setEnabled(true);  // Abilita il tasto stop
}
