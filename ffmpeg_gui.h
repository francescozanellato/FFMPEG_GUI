#ifndef FFMPEG_GUI_H
#define FFMPEG_GUI_H

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QProcess>
#include <QCloseEvent>

// Sottoclasse per abilitare il drag-and-drop sui campi di testo
class DropLineEdit : public QLineEdit {
    Q_OBJECT
public:
    explicit DropLineEdit(QWidget *parent = nullptr);

signals:
    void fileDropped();

protected:
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dropEvent(QDropEvent *e) override;
};

class FFmpegGUI : public QWidget {
    Q_OBJECT
public:
    explicit FFmpegGUI(QWidget *parent = nullptr);

private slots:
    void browseVideo();
    void browseAudio1();
    void browseAudio2();
    void browseAudio3();
    void browseOutput();
    void loadSettings();
    void executeCommand();
    void stopProcess();
    void fill_defaultEditOutputField();
    void readProcessOutput();
    void sendConsoleInput();
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void openSettingsFile();
    void aboutQt();
    void showLicense();

private:
    QString browseFile(bool save = false);
    DropLineEdit *editVideo, *editAudio1, *editAudio2, *editAudio3, *editOutput;
    QComboBox *comboOptions, *comboCombinations;
    QPushButton *btnRun;
    QPushButton *btnStop;
    QPushButton *btnSettings;
    QPushButton *btnLicense;
    QPushButton *btnAboutQt;


    QString ffmpegPath;

    QProcess *ffmpegProcess;
    QPlainTextEdit *consoleOutput;
    QLineEdit *consoleInput;

protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // FFMPEG_GUI_H
