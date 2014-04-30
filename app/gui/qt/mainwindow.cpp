//--
// This file is part of Sonic Pi: http://sonic-pi.net
// Full project source: https://github.com/samaaron/sonic-pi
// License: https://github.com/samaaron/sonic-pi/blob/master/LICENSE.md
//
// Copyright 2013, 2014 by Sam Aaron (http://sam.aaron.name).
// All rights reserved.
//
// Permission is granted for use, copying, modification, distribution,
// and distribution of modified versions of this work as long as this
// notice is included.
//++
#include <sstream>
#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QIcon>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QDockWidget>
#include <QPoint>
#include <QSettings>
#include <QSize>
#include <QStatusBar>
#include <QTextEdit>
#include <QToolBar>
#include <QProcess>
#include <QFont>
#include <QTabWidget>
#include <QString>
#include <QTextStream>
#include <QSplashScreen>
#include <QPixmap>
#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QGridLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QScrollArea>
#include <Qsci/qsciapis.h>
#include <Qsci/qsciscintilla.h>
#include <sonicpilexer.h>
#include <iostream>
#include "oscpkt.hh"
#include "udp.hh"

#if defined(Q_OS_WIN)
  #include <QtConcurrentRun>
#elif defined(Q_OS_MAC)
  #include "qtconcurrent/qtconcurrent"
#else
  //assuming Raspberry Pi
  #include <cmath>
  #include <QtConcurrentRun>
#endif


#include "mainwindow.h"

using namespace oscpkt;

MainWindow::MainWindow(QApplication &app, QSplashScreen &splash) {

  this->setUnifiedTitleAndToolBarOnMac(true);

  is_recording = false;
  server_started = false;
  show_rec_icon_a = false;
  cont_listening_for_osc = true;
  osc_incoming_port_open = false;

  rec_flash_timer = new QTimer(this);
  connect(rec_flash_timer, SIGNAL(timeout()), this, SLOT(toggleRecordingOnIcon()));

  QtConcurrent::run(this, &MainWindow::startOSCListener);
  serverProcess = new QProcess();

  QString serverProgram = "ruby " + QCoreApplication::applicationDirPath() + "/../../server/bin/start-server.rb";
  std::cerr << serverProgram.toStdString() << std::endl;
  serverProcess->start(serverProgram);
  serverProcess->waitForStarted();

  tabs = new QTabWidget();
  tabs->setTabsClosable(false);
  tabs->setMovable(false);
  tabs->setTabPosition(QTabWidget::South);
  setCentralWidget(tabs);

  workspace1 = new QsciScintilla;
  workspace2 = new QsciScintilla;
  workspace3 = new QsciScintilla;
  workspace4 = new QsciScintilla;
  workspace5 = new QsciScintilla;
  workspace6 = new QsciScintilla;
  workspace7 = new QsciScintilla;
  workspace8 = new QsciScintilla;

  QString w1 = "Workspace 1";
  QString w2 = "Workspace 2";
  QString w3 = "Workspace 3";
  QString w4 = "Workspace 4";
  QString w5 = "Workspace 5";
  QString w6 = "Workspace 6";
  QString w7 = "Workspace 7";
  QString w8 = "Workspace 8";

  tabs->addTab(workspace1, w1);
  tabs->addTab(workspace2, w2);
  tabs->addTab(workspace3, w3);
  tabs->addTab(workspace4, w4);
  tabs->addTab(workspace5, w5);
  tabs->addTab(workspace6, w6);
  tabs->addTab(workspace7, w7);
  tabs->addTab(workspace8, w8);

  lexer = new SonicPiLexer;
  lexer->setAutoIndentStyle(QsciScintilla::AiMaintain);

  QsciAPIs* api = new QsciAPIs(lexer);
  api->add("drum_heavy_kick");
  api->add("drum_tom_mid_soft");
  api->add("drum_tom_mid_hard");
  api->add("drum_tom_lo_soft");
  api->add("drum_tom_lo_hard");
  api->add("drum_tom_hi_soft");
  api->add("drum_tom_hi_hard");
  api->add("drum_splash_soft");
  api->add("drum_splash_hard");
  api->add("drum_snare_soft");
  api->add("drum_snare_hard");
  api->add("drum_cymbal_soft");
  api->add("drum_cymbal_hard");
  api->add("drum_cymbal_open");
  api->add("drum_cymbal_closed");
  api->add("drum_cymbal_pedal");
  api->add("drum_bass_soft");
  api->add("drum_bass_hard");

  api->add("elec_triangle");
  api->add("elec_snare");
  api->add("elec_lo_snare");
  api->add("elec_mid_snare");
  api->add("elec_hi_snare");
  api->add("elec_cymbal");
  api->add("elec_soft_kick");
  api->add("elec_filt_snare");
  api->add("elec_fuzz_tom");
  api->add("elec_chime");
  api->add("elec_bong");
  api->add("elec_twang");
  api->add("elec_wood");
  api->add("elec_pop");
  api->add("elec_beep");
  api->add("elec_blip");
  api->add("elec_blip2");
  api->add("elec_ping");
  api->add("elec_bell");
  api->add("elec_flip");
  api->add("elec_tick");
  api->add("elec_hollow_kick");
  api->add("elec_twip");
  api->add("elec_plip");
  api->add("elec_blup");

  api->add("guit_harmonics");
  api->add("guit_e_fifths");
  api->add("guit_e_slide");


  api->add("misc_burp");

  api->add("perc_bell");

  api->add("ambi_soft_buzz");
  api->add("ambi_swoosh");
  api->add("ambi_drone");
  api->add("ambi_glass_hum");
  api->add("ambi_glass_rub");
  api->add("ambi_haunted_hum");
  api->add("ambi_piano");
  api->add("ambi_lunar_land");
  api->add("ambi_dark_woosh");
  api->add("ambi_choir");

  api->add("bass_hit_c");
  api->add("bass_hard_c");
  api->add("bass_thick_c");
  api->add("bass_drop_c");
  api->add("bass_woodsy_c");
  api->add("bass_voxy_c");
  api->add("bass_voxy_hit_c");
  api->add("bass_dnb_f");

  api->add("loop_industrial");
  api->add("loop_compus");
  api->add("loop_amen");
  api->add("loop_amen_full");

  api->add("with_fx");
  api->add("with_synth");
  api->add("with_debug");
  api->add("with_arg_checks");
  api->add("with_merged_synth_defaults");
  api->add("with_synth_defaults");
  api->add("with_sample_pack");
  api->prepare();

  QFont font("Monospace");
  font.setStyleHint(QFont::Monospace);
  lexer->setDefaultFont(font);

  outputPane = new QTextEdit;
  errorPane = new QTextEdit;

  outputPane->setReadOnly(true);
  errorPane->setReadOnly(true);
  outputPane->document()->setMaximumBlockCount(1000);
  errorPane->document()->setMaximumBlockCount(1000);

  outputPane->zoomIn(1);
  errorPane->zoomIn(1);

  prefsWidget = new QDockWidget(tr("Preferences"), this);
  prefsWidget->setAllowedAreas(Qt::RightDockWidgetArea);
  prefsCentral = new QWidget;
  prefsWidget->setWidget(prefsCentral);
  addDockWidget(Qt::RightDockWidgetArea, prefsWidget);
  prefsWidget->hide();

  outputWidget = new QDockWidget(tr("Output"), this);
  outputWidget->setAllowedAreas(Qt::RightDockWidgetArea);
  outputWidget->setWidget(outputPane);
  addDockWidget(Qt::RightDockWidgetArea, outputWidget);

  errorWidget = new QDockWidget(tr("Errors"), this);
  errorWidget->setAllowedAreas(Qt::RightDockWidgetArea);
  errorWidget->setWidget(errorPane);
  addDockWidget(Qt::RightDockWidgetArea, errorWidget);


  docsCentral = new QTabWidget;
  docsCentral->setTabsClosable(false);
  docsCentral->setMovable(false);
  docsCentral->setTabPosition(QTabWidget::West);
  docWidget = new QDockWidget("Documentation", this);
  docWidget->setAllowedAreas(Qt::TopDockWidgetArea);
  docWidget->setWidget(docsCentral);

  langDocPane = new QTextEdit;
  langDocPane->setReadOnly(true);
  synthsDocPane = new QTextEdit;
  synthsDocPane->setReadOnly(true);
  fxDocPane = new QTextEdit;
  fxDocPane->setReadOnly(true);
  samplesDocPane = new QTextEdit;
  samplesDocPane->setReadOnly(true);
  examplesDocPane = new QTextEdit;
  examplesDocPane->setReadOnly(true);

  addDockWidget(Qt::TopDockWidgetArea, docWidget);
  docWidget->hide();

  initWorkspace(workspace1);
  initWorkspace(workspace2);
  initWorkspace(workspace3);
  initWorkspace(workspace4);
  initWorkspace(workspace5);
  initWorkspace(workspace6);
  initWorkspace(workspace7);
  initWorkspace(workspace8);

  createActions();
  createToolBars();
  createStatusBar();

  readSettings();

  setWindowTitle(tr("Sonic Pi"));

  connect(&app, SIGNAL( aboutToQuit() ), this, SLOT( onExitCleanup() ) );

  while (!server_started && cont_listening_for_osc) {
    sleep(1);
    if(osc_incoming_port_open) {
      Message msg("/ping");
      msg.pushStr("QtClient/1/hello");
      sendOSC(msg);
    }
  }

  loadWorkspaces();

  systemVol = new QSlider(this);
  connect(systemVol, SIGNAL(valueChanged(int)), this, SLOT(changeSystemVol(int)));
  initPrefsWindow();
  initDocsWindow();
  this->show();
  splash.finish(this);
}

void MainWindow::showOutputPane() {
  outputWidget->show();
}

void MainWindow::showErrorPane() {
  errorWidget->show();
}


void MainWindow::initPrefsWindow() {

  QGridLayout *grid = new QGridLayout;

  QGroupBox *volBox = new QGroupBox(tr("System Volume"));
  volBox->setToolTip("Use this slider to change the system volume of your Raspberry Pi");
  QGroupBox *groupBox = new QGroupBox(tr("Force Audio Output"));
  groupBox->setToolTip("Your Raspberry Pi has two forms of audio output. \nFirstly, there is the headphone jack of the Raspberry Pi itself. \nSecondly, some HDMI monitors/TVs support audio through the HDMI port. \nUse these buttons to force the output to the one you want. \nFor example, if you have headphones connected to your Raspberry Pi, choose 'Headphones'. ");
  QRadioButton *radio1 = new QRadioButton(tr("&Default"));
  QRadioButton *radio2 = new QRadioButton(tr("&Headphones"));
  QRadioButton *radio3 = new QRadioButton(tr("&HDMI"));
  radio1->setChecked(true);

  connect(radio1, SIGNAL(clicked()), this, SLOT(setSystemAudioAuto()));
  connect(radio2, SIGNAL(clicked()), this, SLOT(setSystemAudioHeadphones()));
  connect(radio3, SIGNAL(clicked()), this, SLOT(setSystemAudioHDMI()));

  QVBoxLayout *audio_box = new QVBoxLayout;
  audio_box->addWidget(radio1);
  audio_box->addWidget(radio2);
  audio_box->addWidget(radio3);
  audio_box->addStretch(1);
  groupBox->setLayout(audio_box);

  QHBoxLayout *vol_box = new QHBoxLayout;
  vol_box->addWidget(systemVol);
  volBox->setLayout(vol_box);

  QGroupBox *showBox = new QGroupBox("Show Panes");
  QPushButton *show_output = new QPushButton("Show Output Pane");
  QPushButton *show_error = new QPushButton("Show Error Pane");

  connect(show_output, SIGNAL(clicked()), this, SLOT(showOutputPane()));
  connect(show_error, SIGNAL(clicked()), this, SLOT(showErrorPane()));

  QVBoxLayout *info_box_layout = new QVBoxLayout;
  info_box_layout->addWidget(show_output);
  info_box_layout->addWidget(show_error);
  showBox->setLayout(info_box_layout);

  QGroupBox *debug_box = new QGroupBox("Debug Options");
  print_output = new QCheckBox("Print output");
  check_args = new QCheckBox("Check synth args");
  print_output->setChecked(true);
  check_args->setChecked(true);

  QVBoxLayout *debug_box_layout = new QVBoxLayout;
  debug_box_layout->addWidget(print_output);
  debug_box_layout->addWidget(check_args);
  debug_box->setLayout(debug_box_layout);

  grid->addWidget(groupBox, 0, 0);
  grid->addWidget(volBox, 0, 1);
  grid->addWidget(showBox, 1, 0);
  grid->addWidget(debug_box, 1, 1);
  prefsCentral->setLayout(grid);
}

void MainWindow::initWorkspace(QsciScintilla* ws) {
  ws->setAutoIndent(true);
  ws->setIndentationsUseTabs(false);
  ws->setBackspaceUnindents(true);
  ws->setTabIndents(true);
  ws->setMatchedBraceBackgroundColor(QColor("dimgray"));
  ws->setMatchedBraceForegroundColor(QColor("white"));

  ws->setIndentationWidth(2);
  ws->setIndentationGuides(true);
  ws->setIndentationGuidesForegroundColor(QColor("deep pink"));
  ws->setBraceMatching( QsciScintilla::SloppyBraceMatch);
  //TODO: add preference toggle for this:
  //ws->setFolding(QsciScintilla::CircledTreeFoldStyle, 2);
  ws->setCaretLineVisible(true);
  ws->setCaretLineBackgroundColor(QColor("whitesmoke"));
  ws->setFoldMarginColors(QColor("whitesmoke"),QColor("whitesmoke"));
  ws->setMarginLineNumbers(0, true);
  ws->setMarginWidth(0, "1000000");
  ws->setMarginsBackgroundColor(QColor("whitesmoke"));
  ws->setMarginsForegroundColor(QColor("dark gray"));
  ws->setMarginsFont(QFont("Menlo",5, -1, true));
  ws->setUtf8(true);
  ws->setText("#loading...");
  ws->setLexer(lexer);
  ws->zoomIn(13);
  ws->setAutoCompletionThreshold(5);
  ws->setAutoCompletionSource(QsciScintilla::AcsAPIs);
  ws->setSelectionBackgroundColor("DeepPink");
  ws->setSelectionForegroundColor("white");
  ws->setCaretWidth(5);
  ws->setCaretForegroundColor("deep pink");

}

void MainWindow::startOSCListener() {
  std::cout << "starting OSC Server" << std::endl;
  int PORT_NUM = 4558;
  UdpSocket sock;
  sock.bindTo(PORT_NUM);
  std::cout << "Listening on port 4558" << std::endl;
  if (!sock.isOk()) {
    std::cout << "Unable to listen to OSC messages on port 4558" << std::endl;
  } else {
    PacketReader pr;
    PacketWriter pw;
    osc_incoming_port_open = true;
    while (sock.isOk() && cont_listening_for_osc) {
      if (sock.receiveNextPacket(30 /* timeout, in ms */)) {
        pr.init(sock.packetData(), sock.packetSize());
        oscpkt::Message *msg;
        while (pr.isOk() && (msg = pr.popMessage()) != 0) {

          if (msg->match("/message")) {
            std::string s;
            if (msg->arg().popStr(s).isOkNoMoreArgs()) {
              // Evil nasties!
              // See: http://www.qtforum.org/article/26801/qt4-threads-and-widgets.html
              QMetaObject::invokeMethod( outputPane, "append", Qt::QueuedConnection,
                                         Q_ARG(QString, QString::fromStdString(s)) );
            } else {
              std::cout << "Server: unhandled message: "<< std::endl;
            }
          }
          else if (msg->match("/error")) {
            std::string desc;
            std::string backtrace;
            if (msg->arg().popStr(desc).popStr(backtrace).isOkNoMoreArgs()) {
              // Evil nasties!
              // See: http://www.qtforum.org/article/26801/qt4-threads-and-widgets.html
              QMetaObject::invokeMethod( errorPane, "clear", Qt::QueuedConnection);
              QMetaObject::invokeMethod( errorPane, "setHtml", Qt::QueuedConnection,
                                         Q_ARG(QString, "<h3><pre>" + QString::fromStdString(desc) + "</pre></h3><pre>" + QString::fromStdString(backtrace) + "</pre>") );

            } else {
              std::cout << "Server: unhandled error: "<< std::endl;
            }
          }
          else if (msg->match("/replace-buffer")) {
            std::string id;
            std::string content;
            if (msg->arg().popStr(id).popStr(content).isOkNoMoreArgs()) {
              QsciScintilla* ws = filenameToWorkspace(id);
              QMetaObject::invokeMethod( ws, "setText", Qt::QueuedConnection,
                                         Q_ARG(QString, QString::fromStdString(content)) );
            } else {
              std::cout << "Server: unhandled replace-buffer: "<< std::endl;
            }
          }
          else if (msg->match("/exited")) {
            if (msg->arg().isOkNoMoreArgs()) {
              std::cout << "server asked us to exit" << std::endl;
              cont_listening_for_osc = false;
            } else {
              std::cout << "Server: unhandled exited: "<< std::endl;
            }
          }
          else if (msg->match("/ack")) {
            std::string id;
            if (msg->arg().popStr(id).isOkNoMoreArgs()) {
              server_started = true;
            } else
              std::cout << "Server: unhandled ack " << std::endl;
          }
          else {
            std::cout << "Unknown message" << std::endl;
          }
        }
      }
    }
  }
  std::cout << "OSC Stopped, releasing socket" << std::endl;
  sock.close();
}



void MainWindow::loadWorkspaces()
{
  std::cout << "loading workspaces" << std::endl;;

  Message msg("/load-buffer");
  msg.pushStr("workspace_one");
  sendOSC(msg);

  Message msg2("/load-buffer");
  msg2.pushStr("workspace_two");
  sendOSC(msg2);

  Message msg3("/load-buffer");
  msg3.pushStr("workspace_three");
  sendOSC(msg3);

  Message msg4("/load-buffer");
  msg4.pushStr("workspace_four");
  sendOSC(msg4);

  Message msg5("/load-buffer");
  msg5.pushStr("workspace_five");
  sendOSC(msg5);

  Message msg6("/load-buffer");
  msg6.pushStr("workspace_six");
  sendOSC(msg6);

  Message msg7("/load-buffer");
  msg7.pushStr("workspace_seven");
  sendOSC(msg7);

  Message msg8("/load-buffer");
  msg8.pushStr("workspace_eight");
  sendOSC(msg8);
}

void MainWindow::saveWorkspaces()
{
  std::cout << "saving workspaces" << std::endl;;

  std::string code = workspace1->text().toStdString();
  Message msg("/save-buffer");
  msg.pushStr("workspace_one");
  msg.pushStr(code);
  sendOSC(msg);

  std::string code2 = workspace2->text().toStdString();
  Message msg2("/save-buffer");
  msg2.pushStr("workspace_two");
  msg2.pushStr(code2);
  sendOSC(msg2);

  std::string code3 = workspace3->text().toStdString();
  Message msg3("/save-buffer");
  msg3.pushStr("workspace_three");
  msg3.pushStr(code3);
  sendOSC(msg3);

  std::string code4 = workspace4->text().toStdString();
  Message msg4("/save-buffer");
  msg4.pushStr("workspace_four");
  msg4.pushStr(code4);
  sendOSC(msg4);

  std::string code5 = workspace5->text().toStdString();
  Message msg5("/save-buffer");
  msg5.pushStr("workspace_five");
  msg5.pushStr(code5);
  sendOSC(msg5);

  std::string code6 = workspace6->text().toStdString();
  Message msg6("/save-buffer");
  msg6.pushStr("workspace_six");
  msg6.pushStr(code6);
  sendOSC(msg6);

  std::string code7 = workspace7->text().toStdString();
  Message msg7("/save-buffer");
  msg7.pushStr("workspace_seven");
  msg7.pushStr(code7);
  sendOSC(msg7);

  std::string code8 = workspace8->text().toStdString();
  Message msg8("/save-buffer");
  msg8.pushStr("workspace_eight");
  msg8.pushStr(code8);
  sendOSC(msg8);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  writeSettings();
  event->accept();
}

QString MainWindow::currentTabLabel()
{
  return tabs->tabText(tabs->currentIndex());
}


bool MainWindow::saveAs()
{
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save Current Workspace"), QDir::homePath() + "/Desktop");
  if(!fileName.isEmpty()){
    return saveFile(fileName, (QsciScintilla*)tabs->currentWidget());
  } else {
    return false;
  }
}

 void MainWindow::sendOSC(Message m)
{
  UdpSocket sock;
  int PORT_NUM = 4557;
  sock.connectTo("localhost", PORT_NUM);
  if (!sock.isOk()) {
    std::cerr << "Error connection to port " << PORT_NUM << ": " << sock.errorMessage() << "\n";
  } else {

    PacketWriter pw;
    pw.addMessage(m);
    sock.sendPacket(pw.packetData(), pw.packetSize());
  }
}

void MainWindow::runCode()
{
  errorPane->clear();
  statusBar()->showMessage(tr("Running...."), 2000);
  std::string code = ((QsciScintilla*)tabs->currentWidget())->text().toStdString();
  Message msg("/save-and-run-buffer");
  std::string filename = workspaceFilename( (QsciScintilla*)tabs->currentWidget());
  msg.pushStr(filename);
  if(!print_output->isChecked()) {
    code = "use_debug false #__nosave__ set by Qt GUI user preferences.\n" + code ;
  }
  if(!check_args->isChecked()) {
    code = "use_arg_checks false #__nosave__ set by Qt GUI user preferences.\n" + code ;
  }

  msg.pushStr(code);
  sendOSC(msg);
}


void MainWindow::stopCode()
{
  stopRunningSynths();
  outputPane->clear();
  errorPane->clear();
  statusBar()->showMessage(tr("Stopping..."), 2000);
}

void MainWindow::about()
{
  infoWindow = new QMainWindow();
  imageLabel = new QLabel(this);
  QPixmap image(":/images/splash.png");

  imageLabel->setPixmap(image);
  infoWindow->setCentralWidget(imageLabel);
  infoWindow->setMinimumHeight(image.height());
  infoWindow->setMaximumHeight(image.height());
  infoWindow->setMinimumWidth(image.width());
  infoWindow->setMaximumWidth(image.width());
  infoWindow->show();
}


void MainWindow::help()
{
  if(docWidget->isVisible()) {
    docWidget->hide();
  } else {
    docWidget->show();
  }
}

void MainWindow::changeSystemVol(int val) {
#if defined(Q_OS_WIN)
  //do nothing
#elif defined(Q_OS_MAC)
  //do nothing, just print out what it would do on RPi
  float v = (float) val;
  float vol_float = pow(v/100.0, (float)1./3.) * 100.0;
  std::ostringstream ss;
  ss << vol_float;
  QString prog = "amixer cset numid=1 " + QString::fromStdString(ss.str()) + '%';
  std::cout << prog.toStdString() << std::endl;
#else
  //assuming Raspberry Pi
  QProcess *p = new QProcess();
  float v = (float) val;
  // handle the fact that the amixer percentage range isn't linear
  float vol_float = std::pow(v/100.0, (float)1./3.) * 100.0;
  std::ostringstream ss;
  ss << vol_float;
  QString prog = "amixer cset numid=1 " + QString::fromStdString(ss.str()) + '%';
  p->start(prog);
#endif

}


void MainWindow::setSystemAudioHeadphones(){

#if defined(Q_OS_WIN)
  //do nothing
#elif defined(Q_OS_MAC)
  //do nothing, just print out what it would do on RPi
  QString prog = "amixer cset numid=3 1";
  std::cout << prog.toStdString() << std::endl;
#else
  //assuming Raspberry Pi
  QProcess *p = new QProcess();
  QString prog = "amixer cset numid=3 1";
  p->start(prog);
#endif
}

void MainWindow::setSystemAudioHDMI(){

#if defined(Q_OS_WIN)
  //do nothing
#elif defined(Q_OS_MAC)
  //do nothing, just print out what it would do on RPi
  QString prog = "amixer cset numid=3 2";
  std::cout << prog.toStdString() << std::endl;
#else
  //assuming Raspberry Pi
  QProcess *p = new QProcess();
  QString prog = "amixer cset numid=3 2";
  p->start(prog);
#endif
}

void MainWindow::setSystemAudioAuto(){

#if defined(Q_OS_WIN)
  //do nothing
#elif defined(Q_OS_MAC)
  //do nothing, just print out what it would do on RPi
  QString prog = "amixer cset numid=3 0";
  std::cout << prog.toStdString() << std::endl;
#else
  //assuming Raspberry Pi
  QProcess *p = new QProcess();
  QString prog = "amixer cset numid=3 0";
  p->start(prog);
#endif
}

void MainWindow::showPrefsPane()
{
  if(prefsWidget->isVisible()) {
    prefsWidget->hide();
  } else {
    prefsWidget->show();
  }
}

void MainWindow::zoomFontIn()
{
  ((QsciScintilla*)tabs->currentWidget())->zoomIn(1);
}

void MainWindow::zoomFontOut()
{
  ((QsciScintilla*)tabs->currentWidget())->zoomOut(1);
}


void MainWindow::documentWasModified()
{
  setWindowModified(textEdit->isModified());
}


void MainWindow::stopRunningSynths()
{
  Message msg("/stop-all-jobs");
  sendOSC(msg);
}

void MainWindow::clearOutputPanels()
{
    outputPane->clear();
    errorPane->clear();
}

void MainWindow::createActions()
{

  runAct = new QAction(QIcon(":/images/run.png"), tr("&Run"), this);
  runAct->setShortcut(tr("Ctrl+R"));
  runAct->setStatusTip(tr("Run code"));
  connect(runAct, SIGNAL(triggered()), this, SLOT(runCode()));

  stopAct = new QAction(QIcon(":/images/stop.png"), tr("&Stop"), this);
  stopAct->setShortcut(tr("Ctrl+Q"));
  stopAct->setStatusTip(tr("Stop code"));
  connect(stopAct, SIGNAL(triggered()), this, SLOT(stopCode()));

  saveAsAct = new QAction(QIcon(":/images/save.png"), tr("&Save &As..."), this);
  saveAsAct->setStatusTip(tr("Save the document under a new name"));
  connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

  infoAct = new QAction(QIcon(":/images/info.png"), tr("&Info"), this);
  infoAct->setStatusTip(tr("See information about Sonic Pi"));
  connect(infoAct, SIGNAL(triggered()), this, SLOT(about()));

  helpAct = new QAction(QIcon(":/images/help.png"), tr("&Help"), this);
  helpAct->setStatusTip(tr("Get help"));
  connect(helpAct, SIGNAL(triggered()), this, SLOT(help()));

  prefsAct = new QAction(QIcon(":/images/prefs.png"), tr("&Prefs"), this);
  prefsAct->setStatusTip(tr("Preferences"));
  connect(prefsAct, SIGNAL(triggered()), this, SLOT(showPrefsPane()));

  recAct = new QAction(QIcon(":/images/rec.png"), tr("&Start &Recording"), this);
  recAct->setStatusTip(tr("Start Recording"));
  connect(recAct, SIGNAL(triggered()), this, SLOT(toggleRecording()));

}

void MainWindow::createToolBars()
{

  QWidget *spacerWidget1 = new QWidget(this);
  spacerWidget1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  spacerWidget1->setVisible(true);

  QWidget *spacerWidget2 = new QWidget(this);
  spacerWidget2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  spacerWidget2->setVisible(true);

  fileToolBar = addToolBar(tr("Run"));
  fileToolBar->setIconSize(QSize(270/3, 109/3));
  fileToolBar->addAction(runAct);
  fileToolBar->addAction(stopAct);
  fileToolBar->addWidget(spacerWidget1);

  saveToolBar = addToolBar(tr("Save"));
  saveToolBar->addWidget(spacerWidget1);
  saveToolBar->setIconSize(QSize(270/3, 109/3));
  saveToolBar->addAction(saveAsAct);
  saveToolBar->addAction(recAct);

  supportToolBar = addToolBar(tr("Support"));
  supportToolBar->addWidget(spacerWidget2);
  supportToolBar->addAction(infoAct);
  supportToolBar->addAction(helpAct);
  supportToolBar->setIconSize(QSize(270/3, 109/3));
  supportToolBar->addAction(prefsAct);
}

 void MainWindow::toggleRecordingOnIcon() {
   show_rec_icon_a = !show_rec_icon_a;
   if(show_rec_icon_a) {
     recAct->setIcon(QIcon(":/images/recording_a.png"));
   } else {
     recAct->setIcon(QIcon(":/images/recording_b.png"));
   }
 }

 void MainWindow::toggleRecording() {
   is_recording = !is_recording;
   if(is_recording) {
     recAct->setStatusTip(tr("Stop Recording"));
     rec_flash_timer->start(500);
     Message msg("/start-recording");
     sendOSC(msg);
   } else {
     rec_flash_timer->stop();
     recAct->setStatusTip(tr("Start Recording"));
     recAct->setIcon(QIcon(":/images/rec.png"));
     Message msg("/stop-recording");
     sendOSC(msg);
     QString fileName = QFileDialog::getSaveFileName(this, tr("Save Recording"), QDir::homePath() + "/Desktop/my-recording.wav");
     if (!fileName.isEmpty()) {
         Message msg("/save-recording");
         msg.pushStr(fileName.toStdString());
         sendOSC(msg);
     } else {
       Message msg("/delete-recording");
       sendOSC(msg);
     }
   }
 }


void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings()
{
    QSettings settings("uk.ac.cam.cl", "Sonic Pi");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    resize(size);
    move(pos);
}

void MainWindow::writeSettings()
{
    QSettings settings("uk.ac.cam.cl", "Sonic Pi");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

void MainWindow::loadFile(const QString &fileName, QsciScintilla* &text)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        QMessageBox::warning(this, tr("Sonic Pi"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    text->setText(in.readAll());
    QApplication::restoreOverrideCursor();
    statusBar()->showMessage(tr("File loaded"), 2000);
}

bool MainWindow::saveFile(const QString &fileName, QsciScintilla* text)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly)) {
        QMessageBox::warning(this, tr("Sonic Pi"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << text->text();
    QApplication::restoreOverrideCursor();
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

 std::string MainWindow::workspaceFilename(QsciScintilla* text)
{
  if(text == workspace1) {return "workspace_one";}
  else if(text == workspace2) {return "workspace_two";}
  else if(text == workspace3) {return "workspace_three";}
  else if(text == workspace4) {return "workspace_four";}
  else if(text == workspace5) {return "workspace_five";}
  else if(text == workspace6) {return "workspace_six";}
  else if(text == workspace7) {return "workspace_seven";}
  else if(text == workspace8) {return "workspace_eight";}
  else {return "default";}
}

 QsciScintilla*  MainWindow::filenameToWorkspace(std::string filename)
{
  if(filename == "workspace_one") {return workspace1;}
  else if(filename == "workspace_two") {return workspace2;}
  else if(filename == "workspace_three") {return workspace3;}
  else if(filename == "workspace_four") {return workspace4;}
  else if(filename == "workspace_five") {return workspace5;}
  else if(filename == "workspace_six") {return workspace6;}
  else if(filename == "workspace_seven") {return workspace7;}
  else if(filename == "workspace_eight") {return workspace8;}
  else {return workspace1;}
}

void MainWindow::onExitCleanup()
{
  if(serverProcess->state() == QProcess::NotRunning) {
    std::cout << "Server process is not running, something is up..." << std::endl;
    cont_listening_for_osc = false;
  } else {
    saveWorkspaces();
    sleep(1);
    std::cout << "Asking server process to exit..." << std::endl;
    Message msg("/exit");
    sendOSC(msg);
  }
  std::cout << "Exiting..." << std::endl;

}
void MainWindow::updateDocPane(QListWidgetItem *cur, QListWidgetItem *prev) {
  QString content = cur->data(32).toString();
  std::cout << "update pane" << content.toStdString() << std::endl;
  langDocPane->setHtml(content);
  synthsDocPane->setHtml(content);
  fxDocPane->setHtml(content);
  samplesDocPane->setHtml(content);
  examplesDocPane->setHtml(content);
}

// AUTO-GENERATED-DOCS
// Do not manually add any code below this comment
// otherwise it may be removed

void MainWindow::initDocsWindow() {
// lang info

QListWidget *langNameList = new QListWidget;
langNameList->setSortingEnabled(true);
connect(langNameList, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), this, SLOT(updateDocPane(QListWidgetItem*, QListWidgetItem*)));
QBoxLayout *langLayout = new QBoxLayout(QBoxLayout::LeftToRight);
langLayout->addWidget(langNameList);
langLayout->addWidget(langDocPane);
langLayout->setStretch(1, 1);
QWidget *langTabWidget = new QWidget;
langTabWidget->setLayout(langLayout);
docsCentral->addTab(langTabWidget, "Lang");

QListWidgetItem *lang_item_1 = new QListWidgetItem("foo");
lang_item_1->setData(32, QVariant("this is foo"));
langNameList->addItem(lang_item_1);

QListWidgetItem *lang_item_2 = new QListWidgetItem("bar");
lang_item_2->setData(32, QVariant("this is bar"));
langNameList->addItem(lang_item_2);

// synths info

QListWidget *synthsNameList = new QListWidget;
synthsNameList->setSortingEnabled(true);
connect(synthsNameList, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), this, SLOT(updateDocPane(QListWidgetItem*, QListWidgetItem*)));
QBoxLayout *synthsLayout = new QBoxLayout(QBoxLayout::LeftToRight);
synthsLayout->addWidget(synthsNameList);
synthsLayout->addWidget(synthsDocPane);
synthsLayout->setStretch(1, 1);
QWidget *synthsTabWidget = new QWidget;
synthsTabWidget->setLayout(synthsLayout);
docsCentral->addTab(synthsTabWidget, "Synths");

QListWidgetItem *synths_item_3 = new QListWidgetItem("Dull Bell");
synths_item_3->setData(32, QVariant("<h2> Dull Bell</h2><h2><pre>use_synth :dull_bell</pre></h2><h4><pre>{note: 52, note_slide: 0, amp: 1, amp_slide: 0, pan: 0, pan_slide: 0, attack: 0.01, sustain: 0, release: 1}</pre></h4><h3>  A simple dull dischordant bell sound.</h3><h3>Argument Documentation:</h3><ul>  <li> note:<ul>    <li> doc: Note to play. Either a MIDI number or a symbol representing a note. For example: 30, 52, :C, :C2, :Eb4, or :Ds3</li>    <li> default: 52</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> note_slide:<ul>    <li> doc: Amount of time (in seconds) for the note to change. A long slide value means that the note takes a long time to slide from the previous note to the new note. A slide of 0 means that the note instantly changes to the new note.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp:<ul>    <li> doc: The amplitude of the sound. Typically a value between 0 and 1. Higher amplitudes may be used, but won't make the sound louder, it will just reduce the quality of all the sounds currently being played (due to compression.)</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp_slide:<ul>    <li> doc: Amount of time (in seconds) for the amplitude (amp) to change. A long slide value means that the amp takes a long time to slide from the previous amplitude to the new amplitude. A slide of 0 means that the amplitude instantly changes to the new amplitude.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> pan:<ul>    <li> doc: Position of sound in stereo. With headphones on, this means how much of the sound is in the left ear, and how much is in the right ear. With a value of -1, the soundis completely in the left ear, a value of 0 puts the sound equally in both ears and a value of 1 puts the sound in the right ear. Values in between -1 and 1 move the sound accordingly.</li>    <li> default: 0</li>    <li> constraints: must be a value between -1 and 1 inclusively</li>    <li>May be changed whilst playing</li></ul>  <li> pan_slide:<ul>    <li> doc: Amount of time (in seconds) for the pan to change. A long slide value means that the pan takes a long time to slide from the previous pan position to the new pan position. A slide of 0 means that the pan instantly changes to the new pan position.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> attack:<ul>    <li> doc: Amount of time (in seconds) for sound to reach full amplitude. A short attack (i.e. 0.01) makes the initial part of the sound very percussive like a sharp tap. A longer attack (i.e 1) fades the sound in gently. Full length of sound is attack + sustain + release.</li>    <li> default: 0.01</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> sustain:<ul>    <li> doc: Amount of time (in seconds) for sound to remain at full amplitude. Longer sustain values result in longer sounds. Full length of sound is attack + sustain + release.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> release:<ul>    <li> doc: Amount of time (in seconds) for sound to move from full amplitude to silent. A short release (i.e. 0.01) makes the final part of the sound very percussive (potentially resulting in a click). A longer release (i.e 1) fades the sound out gently. Full length of sound is attack + sustain + release.</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul></li></ul>"));
synthsNameList->addItem(synths_item_3);

QListWidgetItem *synths_item_4 = new QListWidgetItem("Pretty Bell");
synths_item_4->setData(32, QVariant("<h2> Pretty Bell</h2><h2><pre>use_synth :pretty_bell</pre></h2><h4><pre>{note: 52, note_slide: 0, amp: 1, amp_slide: 0, pan: 0, pan_slide: 0, attack: 0.01, sustain: 0, release: 1}</pre></h4><h3>  A simple pretty bell sound.</h3><h3>Argument Documentation:</h3><ul>  <li> note:<ul>    <li> doc: Note to play. Either a MIDI number or a symbol representing a note. For example: 30, 52, :C, :C2, :Eb4, or :Ds3</li>    <li> default: 52</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> note_slide:<ul>    <li> doc: Amount of time (in seconds) for the note to change. A long slide value means that the note takes a long time to slide from the previous note to the new note. A slide of 0 means that the note instantly changes to the new note.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp:<ul>    <li> doc: The amplitude of the sound. Typically a value between 0 and 1. Higher amplitudes may be used, but won't make the sound louder, it will just reduce the quality of all the sounds currently being played (due to compression.)</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp_slide:<ul>    <li> doc: Amount of time (in seconds) for the amplitude (amp) to change. A long slide value means that the amp takes a long time to slide from the previous amplitude to the new amplitude. A slide of 0 means that the amplitude instantly changes to the new amplitude.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> pan:<ul>    <li> doc: Position of sound in stereo. With headphones on, this means how much of the sound is in the left ear, and how much is in the right ear. With a value of -1, the soundis completely in the left ear, a value of 0 puts the sound equally in both ears and a value of 1 puts the sound in the right ear. Values in between -1 and 1 move the sound accordingly.</li>    <li> default: 0</li>    <li> constraints: must be a value between -1 and 1 inclusively</li>    <li>May be changed whilst playing</li></ul>  <li> pan_slide:<ul>    <li> doc: Amount of time (in seconds) for the pan to change. A long slide value means that the pan takes a long time to slide from the previous pan position to the new pan position. A slide of 0 means that the pan instantly changes to the new pan position.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> attack:<ul>    <li> doc: Amount of time (in seconds) for sound to reach full amplitude. A short attack (i.e. 0.01) makes the initial part of the sound very percussive like a sharp tap. A longer attack (i.e 1) fades the sound in gently. Full length of sound is attack + sustain + release.</li>    <li> default: 0.01</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> sustain:<ul>    <li> doc: Amount of time (in seconds) for sound to remain at full amplitude. Longer sustain values result in longer sounds. Full length of sound is attack + sustain + release.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> release:<ul>    <li> doc: Amount of time (in seconds) for sound to move from full amplitude to silent. A short release (i.e. 0.01) makes the final part of the sound very percussive (potentially resulting in a click). A longer release (i.e 1) fades the sound out gently. Full length of sound is attack + sustain + release.</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul></li></ul>"));
synthsNameList->addItem(synths_item_4);

QListWidgetItem *synths_item_5 = new QListWidgetItem("Saw Wave");
synths_item_5->setData(32, QVariant("<h2> Saw Wave</h2><h2><pre>use_synth :saw_beep</pre></h2><h4><pre>{note: 52, note_slide: 0, amp: 1, amp_slide: 0, pan: 0, pan_slide: 0, attack: 0.0, sustain: 0, release: 0.2}</pre></h4><h3>  A simple saw wave with a low pass filter.</h3><h3>Argument Documentation:</h3><ul>  <li> note:<ul>    <li> doc: Note to play. Either a MIDI number or a symbol representing a note. For example: 30, 52, :C, :C2, :Eb4, or :Ds3</li>    <li> default: 52</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> note_slide:<ul>    <li> doc: Amount of time (in seconds) for the note to change. A long slide value means that the note takes a long time to slide from the previous note to the new note. A slide of 0 means that the note instantly changes to the new note.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp:<ul>    <li> doc: The amplitude of the sound. Typically a value between 0 and 1. Higher amplitudes may be used, but won't make the sound louder, it will just reduce the quality of all the sounds currently being played (due to compression.)</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp_slide:<ul>    <li> doc: Amount of time (in seconds) for the amplitude (amp) to change. A long slide value means that the amp takes a long time to slide from the previous amplitude to the new amplitude. A slide of 0 means that the amplitude instantly changes to the new amplitude.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> pan:<ul>    <li> doc: Position of sound in stereo. With headphones on, this means how much of the sound is in the left ear, and how much is in the right ear. With a value of -1, the soundis completely in the left ear, a value of 0 puts the sound equally in both ears and a value of 1 puts the sound in the right ear. Values in between -1 and 1 move the sound accordingly.</li>    <li> default: 0</li>    <li> constraints: must be a value between -1 and 1 inclusively</li>    <li>May be changed whilst playing</li></ul>  <li> pan_slide:<ul>    <li> doc: Amount of time (in seconds) for the pan to change. A long slide value means that the pan takes a long time to slide from the previous pan position to the new pan position. A slide of 0 means that the pan instantly changes to the new pan position.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> attack:<ul>    <li> doc: Amount of time (in seconds) for sound to reach full amplitude. A short attack (i.e. 0.01) makes the initial part of the sound very percussive like a sharp tap. A longer attack (i.e 1) fades the sound in gently. Full length of sound is attack + sustain + release.</li>    <li> default: 0.0</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> sustain:<ul>    <li> doc: Amount of time (in seconds) for sound to remain at full amplitude. Longer sustain values result in longer sounds. Full length of sound is attack + sustain + release.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> release:<ul>    <li> doc: Amount of time (in seconds) for sound to move from full amplitude to silent. A short release (i.e. 0.01) makes the final part of the sound very percussive (potentially resulting in a click). A longer release (i.e 1) fades the sound out gently. Full length of sound is attack + sustain + release.</li>    <li> default: 0.2</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul></li></ul>"));
synthsNameList->addItem(synths_item_5);

QListWidgetItem *synths_item_6 = new QListWidgetItem("Sine Wave");
synths_item_6->setData(32, QVariant("<h2> Sine Wave</h2><h2><pre>use_synth :beep</pre></h2><h4><pre>{note: 52, note_slide: 0, amp: 1, amp_slide: 0, pan: 0, pan_slide: 0, attack: 0.0, sustain: 0, release: 0.2}</pre></h4><h3>  A simple pure sine wave.</h3><h3>Argument Documentation:</h3><ul>  <li> note:<ul>    <li> doc: Note to play. Either a MIDI number or a symbol representing a note. For example: 30, 52, :C, :C2, :Eb4, or :Ds3</li>    <li> default: 52</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> note_slide:<ul>    <li> doc: Amount of time (in seconds) for the note to change. A long slide value means that the note takes a long time to slide from the previous note to the new note. A slide of 0 means that the note instantly changes to the new note.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp:<ul>    <li> doc: The amplitude of the sound. Typically a value between 0 and 1. Higher amplitudes may be used, but won't make the sound louder, it will just reduce the quality of all the sounds currently being played (due to compression.)</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp_slide:<ul>    <li> doc: Amount of time (in seconds) for the amplitude (amp) to change. A long slide value means that the amp takes a long time to slide from the previous amplitude to the new amplitude. A slide of 0 means that the amplitude instantly changes to the new amplitude.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> pan:<ul>    <li> doc: Position of sound in stereo. With headphones on, this means how much of the sound is in the left ear, and how much is in the right ear. With a value of -1, the soundis completely in the left ear, a value of 0 puts the sound equally in both ears and a value of 1 puts the sound in the right ear. Values in between -1 and 1 move the sound accordingly.</li>    <li> default: 0</li>    <li> constraints: must be a value between -1 and 1 inclusively</li>    <li>May be changed whilst playing</li></ul>  <li> pan_slide:<ul>    <li> doc: Amount of time (in seconds) for the pan to change. A long slide value means that the pan takes a long time to slide from the previous pan position to the new pan position. A slide of 0 means that the pan instantly changes to the new pan position.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> attack:<ul>    <li> doc: Amount of time (in seconds) for sound to reach full amplitude. A short attack (i.e. 0.01) makes the initial part of the sound very percussive like a sharp tap. A longer attack (i.e 1) fades the sound in gently. Full length of sound is attack + sustain + release.</li>    <li> default: 0.0</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> sustain:<ul>    <li> doc: Amount of time (in seconds) for sound to remain at full amplitude. Longer sustain values result in longer sounds. Full length of sound is attack + sustain + release.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> release:<ul>    <li> doc: Amount of time (in seconds) for sound to move from full amplitude to silent. A short release (i.e. 0.01) makes the final part of the sound very percussive (potentially resulting in a click). A longer release (i.e 1) fades the sound out gently. Full length of sound is attack + sustain + release.</li>    <li> default: 0.2</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul></li></ul>"));
synthsNameList->addItem(synths_item_6);

QListWidgetItem *synths_item_7 = new QListWidgetItem("Detuned Saw wave");
synths_item_7->setData(32, QVariant("<h2> Detuned Saw wave</h2><h2><pre>use_synth :dsaw</pre></h2><h4><pre>{note: 52, note_slide: 0, amp: 1, amp_slide: 0, pan: 0, pan_slide: 0, attack: 0.1, sustain: 0, release: 0.3, cutoff: 100, cutoff_slide: 0, detune: 0.1, detune_slide: 0}</pre></h4><h3>  A pair of detuned saw waves with a lop pass filter.</h3><h3>Argument Documentation:</h3><ul>  <li> note:<ul>    <li> doc: Note to play. Either a MIDI number or a symbol representing a note. For example: 30, 52, :C, :C2, :Eb4, or :Ds3</li>    <li> default: 52</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> note_slide:<ul>    <li> doc: Amount of time (in seconds) for the note to change. A long slide value means that the note takes a long time to slide from the previous note to the new note. A slide of 0 means that the note instantly changes to the new note.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp:<ul>    <li> doc: The amplitude of the sound. Typically a value between 0 and 1. Higher amplitudes may be used, but won't make the sound louder, it will just reduce the quality of all the sounds currently being played (due to compression.)</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp_slide:<ul>    <li> doc: Amount of time (in seconds) for the amplitude (amp) to change. A long slide value means that the amp takes a long time to slide from the previous amplitude to the new amplitude. A slide of 0 means that the amplitude instantly changes to the new amplitude.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> pan:<ul>    <li> doc: Position of sound in stereo. With headphones on, this means how much of the sound is in the left ear, and how much is in the right ear. With a value of -1, the soundis completely in the left ear, a value of 0 puts the sound equally in both ears and a value of 1 puts the sound in the right ear. Values in between -1 and 1 move the sound accordingly.</li>    <li> default: 0</li>    <li> constraints: must be a value between -1 and 1 inclusively</li>    <li>May be changed whilst playing</li></ul>  <li> pan_slide:<ul>    <li> doc: Amount of time (in seconds) for the pan to change. A long slide value means that the pan takes a long time to slide from the previous pan position to the new pan position. A slide of 0 means that the pan instantly changes to the new pan position.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> attack:<ul>    <li> doc: Amount of time (in seconds) for sound to reach full amplitude. A short attack (i.e. 0.01) makes the initial part of the sound very percussive like a sharp tap. A longer attack (i.e 1) fades the sound in gently. Full length of sound is attack + sustain + release.</li>    <li> default: 0.1</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> sustain:<ul>    <li> doc: Amount of time (in seconds) for sound to remain at full amplitude. Longer sustain values result in longer sounds. Full length of sound is attack + sustain + release.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> release:<ul>    <li> doc: Amount of time (in seconds) for sound to move from full amplitude to silent. A short release (i.e. 0.01) makes the final part of the sound very percussive (potentially resulting in a click). A longer release (i.e 1) fades the sound out gently. Full length of sound is attack + sustain + release.</li>    <li> default: 0.3</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> cutoff:<ul>    <li> doc: MIDI note representing the highest frequences allowed to be present in the sound. A low value like 30 makes the sound round and dull, a high value like 100 makes the sound buzzy and crispy.</li>    <li> default: 100</li>    <li> constraints: must be zero or greater,must be a value less than 130</li>    <li>May be changed whilst playing</li></ul>  <li> cutoff_slide:<ul>    <li> doc: Amount of time (in seconds) for the cutoff value to change. A long cutoff_slide value means that the cutoff takes a long time to slide from the previous value to the new value. A cutoff_slide of 0 means that the cutoff instantly changes to the new value.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> detune:<ul>    <li> doc: Distance (in MIDI notes) between components of sound. Affects thickness, sense of tuning and harmony. Tiny values such as 0.1 create a thick sound. Larger values such as 0.5 make the tuning sound strange. Even bigger values such as 5 create chord-like sounds.</li>    <li> default: 0.1</li>    <li> constraints: none</li>    <li>May be changed whilst playing</li></ul>  <li> detune_slide:<ul>    <li> doc: Amount of time (in seconds) for the detune value to change. A long detune_slide value means that the detune takes a long time to slide from the previous value to the new value. A detune_slide of 0 means that the detune instantly changes to the new value.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul></li></ul>"));
synthsNameList->addItem(synths_item_7);

QListWidgetItem *synths_item_8 = new QListWidgetItem("Basic FM synthesis");
synths_item_8->setData(32, QVariant("<h2> Basic FM synthesis</h2><h2><pre>use_synth :fm</pre></h2><h4><pre>{note: 52, note_slide: 0, amp: 1, amp_slide: 0, pan: 0, pan_slide: 0, attack: 1, sustain: 0, release: 1, divisor: 2, divisor_slide: 0, depth: 1, depth_slide: 0}</pre></h4><h3>  A sine wave with a fundamental frequency which is modulated at audio rate by another sine wave with a specific modulation division and depth.</h3><h3>Argument Documentation:</h3><ul>  <li> note:<ul>    <li> doc: Note to play. Either a MIDI number or a symbol representing a note. For example: 30, 52, :C, :C2, :Eb4, or :Ds3</li>    <li> default: 52</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> note_slide:<ul>    <li> doc: Amount of time (in seconds) for the note to change. A long slide value means that the note takes a long time to slide from the previous note to the new note. A slide of 0 means that the note instantly changes to the new note.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp:<ul>    <li> doc: The amplitude of the sound. Typically a value between 0 and 1. Higher amplitudes may be used, but won't make the sound louder, it will just reduce the quality of all the sounds currently being played (due to compression.)</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp_slide:<ul>    <li> doc: Amount of time (in seconds) for the amplitude (amp) to change. A long slide value means that the amp takes a long time to slide from the previous amplitude to the new amplitude. A slide of 0 means that the amplitude instantly changes to the new amplitude.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> pan:<ul>    <li> doc: Position of sound in stereo. With headphones on, this means how much of the sound is in the left ear, and how much is in the right ear. With a value of -1, the soundis completely in the left ear, a value of 0 puts the sound equally in both ears and a value of 1 puts the sound in the right ear. Values in between -1 and 1 move the sound accordingly.</li>    <li> default: 0</li>    <li> constraints: must be a value between -1 and 1 inclusively</li>    <li>May be changed whilst playing</li></ul>  <li> pan_slide:<ul>    <li> doc: Amount of time (in seconds) for the pan to change. A long slide value means that the pan takes a long time to slide from the previous pan position to the new pan position. A slide of 0 means that the pan instantly changes to the new pan position.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> attack:<ul>    <li> doc: Amount of time (in seconds) for sound to reach full amplitude. A short attack (i.e. 0.01) makes the initial part of the sound very percussive like a sharp tap. A longer attack (i.e 1) fades the sound in gently. Full length of sound is attack + sustain + release.</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> sustain:<ul>    <li> doc: Amount of time (in seconds) for sound to remain at full amplitude. Longer sustain values result in longer sounds. Full length of sound is attack + sustain + release.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> release:<ul>    <li> doc: Amount of time (in seconds) for sound to move from full amplitude to silent. A short release (i.e. 0.01) makes the final part of the sound very percussive (potentially resulting in a click). A longer release (i.e 1) fades the sound out gently. Full length of sound is attack + sustain + release.</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> divisor:<ul>    <li> doc: Modifies the frequency of the modulator oscillator relative to the carrier. Don't worry too much about what this means - just try different numbers out!</li>    <li> default: 2</li>    <li> constraints: none</li>    <li>May be changed whilst playing</li></ul>  <li> divisor_slide:<ul>    <li> doc: Amount of time (in seconds) for the divisor value to change. A long divisor_slide value means that the divisor takes a long time to slide from the previous value to the new value. A divisor_slide of 0 means that the divisor instantly changes to the new value.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> depth:<ul>    <li> doc: Modifies the depth of the carrier wave used to modify fundamental frequency. Don't worry too much about what this means - just try different numbers out!</li>    <li> default: 1</li>    <li> constraints: none</li>    <li>May be changed whilst playing</li></ul>  <li> depth_slide:<ul>    <li> doc: Amount of time (in seconds) for the depth value to change. A long depth_slide value means that the depth takes a long time to slide from the previous value to the new value. A depth_slide of 0 means that the depth instantly changes to the new value.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul></li></ul>"));
synthsNameList->addItem(synths_item_8);

QListWidgetItem *synths_item_9 = new QListWidgetItem("Modulated Saw Wave");
synths_item_9->setData(32, QVariant("<h2> Modulated Saw Wave</h2><h2><pre>use_synth :mod_saw</pre></h2><h4><pre>{note: 52, note_slide: 0, amp: 1, amp_slide: 0, pan: 0, pan_slide: 0, attack: 0.01, sustain: 0, release: 2, cutoff: 100, cutoff_slide: 0, mod_rate: 1, mod_rate_slide: 0, mod_range: 5, mod_range__slide: 0, mod_width: 0.5, mod_width_slide: 0}</pre></h4><h3>  A saw wave which modulates between two separate notes.</h3><h3>Argument Documentation:</h3><ul>  <li> note:<ul>    <li> doc: Note to play. Either a MIDI number or a symbol representing a note. For example: 30, 52, :C, :C2, :Eb4, or :Ds3</li>    <li> default: 52</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> note_slide:<ul>    <li> doc: Amount of time (in seconds) for the note to change. A long slide value means that the note takes a long time to slide from the previous note to the new note. A slide of 0 means that the note instantly changes to the new note.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp:<ul>    <li> doc: The amplitude of the sound. Typically a value between 0 and 1. Higher amplitudes may be used, but won't make the sound louder, it will just reduce the quality of all the sounds currently being played (due to compression.)</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp_slide:<ul>    <li> doc: Amount of time (in seconds) for the amplitude (amp) to change. A long slide value means that the amp takes a long time to slide from the previous amplitude to the new amplitude. A slide of 0 means that the amplitude instantly changes to the new amplitude.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> pan:<ul>    <li> doc: Position of sound in stereo. With headphones on, this means how much of the sound is in the left ear, and how much is in the right ear. With a value of -1, the soundis completely in the left ear, a value of 0 puts the sound equally in both ears and a value of 1 puts the sound in the right ear. Values in between -1 and 1 move the sound accordingly.</li>    <li> default: 0</li>    <li> constraints: must be a value between -1 and 1 inclusively</li>    <li>May be changed whilst playing</li></ul>  <li> pan_slide:<ul>    <li> doc: Amount of time (in seconds) for the pan to change. A long slide value means that the pan takes a long time to slide from the previous pan position to the new pan position. A slide of 0 means that the pan instantly changes to the new pan position.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> attack:<ul>    <li> doc: Amount of time (in seconds) for sound to reach full amplitude. A short attack (i.e. 0.01) makes the initial part of the sound very percussive like a sharp tap. A longer attack (i.e 1) fades the sound in gently. Full length of sound is attack + sustain + release.</li>    <li> default: 0.01</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> sustain:<ul>    <li> doc: Amount of time (in seconds) for sound to remain at full amplitude. Longer sustain values result in longer sounds. Full length of sound is attack + sustain + release.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> release:<ul>    <li> doc: Amount of time (in seconds) for sound to move from full amplitude to silent. A short release (i.e. 0.01) makes the final part of the sound very percussive (potentially resulting in a click). A longer release (i.e 1) fades the sound out gently. Full length of sound is attack + sustain + release.</li>    <li> default: 2</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> cutoff:<ul>    <li> doc: MIDI note representing the highest frequences allowed to be present in the sound. A low value like 30 makes the sound round and dull, a high value like 100 makes the sound buzzy and crispy.</li>    <li> default: 100</li>    <li> constraints: must be zero or greater,must be a value less than 130</li>    <li>May be changed whilst playing</li></ul>  <li> cutoff_slide:<ul>    <li> doc: Amount of time (in seconds) for the cutoff value to change. A long cutoff_slide value means that the cutoff takes a long time to slide from the previous value to the new value. A cutoff_slide of 0 means that the cutoff instantly changes to the new value.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> mod_rate:<ul>    <li> doc: Number of times per second that the note switches between the two notes.</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> mod_rate_slide:<ul>    <li> doc: Amount of time (in seconds) for the mod_rate value to change. A long mod_rate_slide value means that the mod_rate takes a long time to slide from the previous value to the new value. A mod_rate_slide of 0 means that the mod_rate instantly changes to the new value.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> mod_range:<ul>    <li> doc: The size of gap between modulation notes. A gap of 12 is one octave.</li>    <li> default: 5</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> mod_range__slide:<ul>    <li> doc: write me</li>    <li> default: 0</li>    <li> constraints: none</li>    <li>Can not be changed once set</li></ul>  <li> mod_width:<ul>    <li> doc: The phase width of the modulation. Represents how even the gap between modulations is.</li>    <li> default: 0.5</li>    <li> constraints: must be a value between 0 and 1 exclusively</li>    <li>May be changed whilst playing</li></ul>  <li> mod_width_slide:<ul>    <li> doc: Amount of time (in seconds) for the mod_width value to change. A long mod_width_slide value means that the mod_width takes a long time to slide from the previous value to the new value. A mod_width_slide of 0 means that the mod_width instantly changes to the new value.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul></li></ul>"));
synthsNameList->addItem(synths_item_9);

QListWidgetItem *synths_item_10 = new QListWidgetItem("Simple Modulated Saw Wave");
synths_item_10->setData(32, QVariant("<h2> Simple Modulated Saw Wave</h2><h2><pre>use_synth :mod_saw_s</pre></h2><h4><pre>{note: 52, amp: 1, pan: 0, attack: 0.01, sustain: 0, release: 2, slide: 0, mod_rate: 1, mod_range: 5, mod_width: 0.5}</pre></h4><h3>  </h3><h3>Argument Documentation:</h3><ul>  <li> note:<ul>    <li> doc: Note to play. Either a MIDI number or a symbol representing a note. For example: 30, 52, :C, :C2, :Eb4, or :Ds3</li>    <li> default: 52</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp:<ul>    <li> doc: The amplitude of the sound. Typically a value between 0 and 1. Higher amplitudes may be used, but won't make the sound louder, it will just reduce the quality of all the sounds currently being played (due to compression.)</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> pan:<ul>    <li> doc: Position of sound in stereo. With headphones on, this means how much of the sound is in the left ear, and how much is in the right ear. With a value of -1, the soundis completely in the left ear, a value of 0 puts the sound equally in both ears and a value of 1 puts the sound in the right ear. Values in between -1 and 1 move the sound accordingly.</li>    <li> default: 0</li>    <li> constraints: must be a value between -1 and 1 inclusively</li>    <li>May be changed whilst playing</li></ul>  <li> attack:<ul>    <li> doc: Amount of time (in seconds) for sound to reach full amplitude. A short attack (i.e. 0.01) makes the initial part of the sound very percussive like a sharp tap. A longer attack (i.e 1) fades the sound in gently. Full length of sound is attack + sustain + release.</li>    <li> default: 0.01</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> sustain:<ul>    <li> doc: Amount of time (in seconds) for sound to remain at full amplitude. Longer sustain values result in longer sounds. Full length of sound is attack + sustain + release.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> release:<ul>    <li> doc: Amount of time (in seconds) for sound to move from full amplitude to silent. A short release (i.e. 0.01) makes the final part of the sound very percussive (potentially resulting in a click). A longer release (i.e 1) fades the sound out gently. Full length of sound is attack + sustain + release.</li>    <li> default: 2</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> slide:<ul>    <li> doc: write me</li>    <li> default: 0</li>    <li> constraints: none</li>    <li>Can not be changed once set</li></ul>  <li> mod_rate:<ul>    <li> doc: Number of times per second that the note switches between the two notes.</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> mod_range:<ul>    <li> doc: The size of gap between modulation notes. A gap of 12 is one octave.</li>    <li> default: 5</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> mod_width:<ul>    <li> doc: The phase width of the modulation. Represents how even the gap between modulations is.</li>    <li> default: 0.5</li>    <li> constraints: must be a value between 0 and 1 exclusively</li>    <li>May be changed whilst playing</li></ul></li></ul>"));
synthsNameList->addItem(synths_item_10);

QListWidgetItem *synths_item_11 = new QListWidgetItem("Modulated Detuned Saw Waves");
synths_item_11->setData(32, QVariant("<h2> Modulated Detuned Saw Waves</h2><h2><pre>use_synth :mod_dsaw</pre></h2><h4><pre>{note: 52, amp: 1, pan: 0, attack: 0.01, sustain: 0, release: 2, slide: 0, cutoff: 100, cutoff_slide: 0, mod_rate: 1, mod_range: 5, mod_width: 0.5, detune: 0.1}</pre></h4><h3>  </h3><h3>Argument Documentation:</h3><ul>  <li> note:<ul>    <li> doc: Note to play. Either a MIDI number or a symbol representing a note. For example: 30, 52, :C, :C2, :Eb4, or :Ds3</li>    <li> default: 52</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp:<ul>    <li> doc: The amplitude of the sound. Typically a value between 0 and 1. Higher amplitudes may be used, but won't make the sound louder, it will just reduce the quality of all the sounds currently being played (due to compression.)</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> pan:<ul>    <li> doc: Position of sound in stereo. With headphones on, this means how much of the sound is in the left ear, and how much is in the right ear. With a value of -1, the soundis completely in the left ear, a value of 0 puts the sound equally in both ears and a value of 1 puts the sound in the right ear. Values in between -1 and 1 move the sound accordingly.</li>    <li> default: 0</li>    <li> constraints: must be a value between -1 and 1 inclusively</li>    <li>May be changed whilst playing</li></ul>  <li> attack:<ul>    <li> doc: Amount of time (in seconds) for sound to reach full amplitude. A short attack (i.e. 0.01) makes the initial part of the sound very percussive like a sharp tap. A longer attack (i.e 1) fades the sound in gently. Full length of sound is attack + sustain + release.</li>    <li> default: 0.01</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> sustain:<ul>    <li> doc: Amount of time (in seconds) for sound to remain at full amplitude. Longer sustain values result in longer sounds. Full length of sound is attack + sustain + release.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> release:<ul>    <li> doc: Amount of time (in seconds) for sound to move from full amplitude to silent. A short release (i.e. 0.01) makes the final part of the sound very percussive (potentially resulting in a click). A longer release (i.e 1) fades the sound out gently. Full length of sound is attack + sustain + release.</li>    <li> default: 2</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> slide:<ul>    <li> doc: write me</li>    <li> default: 0</li>    <li> constraints: none</li>    <li>Can not be changed once set</li></ul>  <li> cutoff:<ul>    <li> doc: MIDI note representing the highest frequences allowed to be present in the sound. A low value like 30 makes the sound round and dull, a high value like 100 makes the sound buzzy and crispy.</li>    <li> default: 100</li>    <li> constraints: must be zero or greater,must be a value less than 130</li>    <li>May be changed whilst playing</li></ul>  <li> cutoff_slide:<ul>    <li> doc: Amount of time (in seconds) for the cutoff value to change. A long cutoff_slide value means that the cutoff takes a long time to slide from the previous value to the new value. A cutoff_slide of 0 means that the cutoff instantly changes to the new value.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> mod_rate:<ul>    <li> doc: Number of times per second that the note switches between the two notes.</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> mod_range:<ul>    <li> doc: The size of gap between modulation notes. A gap of 12 is one octave.</li>    <li> default: 5</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> mod_width:<ul>    <li> doc: The phase width of the modulation. Represents how even the gap between modulations is.</li>    <li> default: 0.5</li>    <li> constraints: must be a value between 0 and 1 exclusively</li>    <li>May be changed whilst playing</li></ul>  <li> detune:<ul>    <li> doc: Distance (in MIDI notes) between components of sound. Affects thickness, sense of tuning and harmony. Tiny values such as 0.1 create a thick sound. Larger values such as 0.5 make the tuning sound strange. Even bigger values such as 5 create chord-like sounds.</li>    <li> default: 0.1</li>    <li> constraints: none</li>    <li>May be changed whilst playing</li></ul></li></ul>"));
synthsNameList->addItem(synths_item_11);

QListWidgetItem *synths_item_12 = new QListWidgetItem("Modulated Detuned Saw Waves Simple");
synths_item_12->setData(32, QVariant("<h2> Modulated Detuned Saw Waves Simple</h2><h2><pre>use_synth :mod_dsaw_s</pre></h2><h4><pre>{note: 52, note_slide: 0, amp: 1, amp_slide: 0, pan: 0, pan_slide: 0, attack: 0.01, sustain: 0, release: 2, mod_rate: 1, mod_rate_slide: 0, mod_range: 5, mod_range_slide: 0, mod_width: 0.5, mod_width_slide: 0, detune: 0.1, detune_slide: 0}</pre></h4><h3>  </h3><h3>Argument Documentation:</h3><ul>  <li> note:<ul>    <li> doc: Note to play. Either a MIDI number or a symbol representing a note. For example: 30, 52, :C, :C2, :Eb4, or :Ds3</li>    <li> default: 52</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> note_slide:<ul>    <li> doc: Amount of time (in seconds) for the note to change. A long slide value means that the note takes a long time to slide from the previous note to the new note. A slide of 0 means that the note instantly changes to the new note.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp:<ul>    <li> doc: The amplitude of the sound. Typically a value between 0 and 1. Higher amplitudes may be used, but won't make the sound louder, it will just reduce the quality of all the sounds currently being played (due to compression.)</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp_slide:<ul>    <li> doc: Amount of time (in seconds) for the amplitude (amp) to change. A long slide value means that the amp takes a long time to slide from the previous amplitude to the new amplitude. A slide of 0 means that the amplitude instantly changes to the new amplitude.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> pan:<ul>    <li> doc: Position of sound in stereo. With headphones on, this means how much of the sound is in the left ear, and how much is in the right ear. With a value of -1, the soundis completely in the left ear, a value of 0 puts the sound equally in both ears and a value of 1 puts the sound in the right ear. Values in between -1 and 1 move the sound accordingly.</li>    <li> default: 0</li>    <li> constraints: must be a value between -1 and 1 inclusively</li>    <li>May be changed whilst playing</li></ul>  <li> pan_slide:<ul>    <li> doc: Amount of time (in seconds) for the pan to change. A long slide value means that the pan takes a long time to slide from the previous pan position to the new pan position. A slide of 0 means that the pan instantly changes to the new pan position.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> attack:<ul>    <li> doc: Amount of time (in seconds) for sound to reach full amplitude. A short attack (i.e. 0.01) makes the initial part of the sound very percussive like a sharp tap. A longer attack (i.e 1) fades the sound in gently. Full length of sound is attack + sustain + release.</li>    <li> default: 0.01</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> sustain:<ul>    <li> doc: Amount of time (in seconds) for sound to remain at full amplitude. Longer sustain values result in longer sounds. Full length of sound is attack + sustain + release.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> release:<ul>    <li> doc: Amount of time (in seconds) for sound to move from full amplitude to silent. A short release (i.e. 0.01) makes the final part of the sound very percussive (potentially resulting in a click). A longer release (i.e 1) fades the sound out gently. Full length of sound is attack + sustain + release.</li>    <li> default: 2</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> mod_rate:<ul>    <li> doc: Number of times per second that the note switches between the two notes.</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> mod_rate_slide:<ul>    <li> doc: Amount of time (in seconds) for the mod_rate value to change. A long mod_rate_slide value means that the mod_rate takes a long time to slide from the previous value to the new value. A mod_rate_slide of 0 means that the mod_rate instantly changes to the new value.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> mod_range:<ul>    <li> doc: The size of gap between modulation notes. A gap of 12 is one octave.</li>    <li> default: 5</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> mod_range_slide:<ul>    <li> doc: Amount of time (in seconds) for the mod_range value to change. A long mod_range_slide value means that the mod_range takes a long time to slide from the previous value to the new value. A mod_range_slide of 0 means that the mod_range instantly changes to the new value.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> mod_width:<ul>    <li> doc: The phase width of the modulation. Represents how even the gap between modulations is.</li>    <li> default: 0.5</li>    <li> constraints: must be a value between 0 and 1 exclusively</li>    <li>May be changed whilst playing</li></ul>  <li> mod_width_slide:<ul>    <li> doc: Amount of time (in seconds) for the mod_width value to change. A long mod_width_slide value means that the mod_width takes a long time to slide from the previous value to the new value. A mod_width_slide of 0 means that the mod_width instantly changes to the new value.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> detune:<ul>    <li> doc: Distance (in MIDI notes) between components of sound. Affects thickness, sense of tuning and harmony. Tiny values such as 0.1 create a thick sound. Larger values such as 0.5 make the tuning sound strange. Even bigger values such as 5 create chord-like sounds.</li>    <li> default: 0.1</li>    <li> constraints: none</li>    <li>May be changed whilst playing</li></ul>  <li> detune_slide:<ul>    <li> doc: Amount of time (in seconds) for the detune value to change. A long detune_slide value means that the detune takes a long time to slide from the previous value to the new value. A detune_slide of 0 means that the detune instantly changes to the new value.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul></li></ul>"));
synthsNameList->addItem(synths_item_12);

QListWidgetItem *synths_item_13 = new QListWidgetItem("Modulated Sine Wave");
synths_item_13->setData(32, QVariant("<h2> Modulated Sine Wave</h2><h2><pre>use_synth :mod_sine</pre></h2><h4><pre>{note: 52, note_slide: 0, amp: 1, pan: 0, attack: 0.01, sustain: 0, release: 2, cutoff: 100, cutoff_slide: 0, mod_rate: 1, mod_rate_slide: 0, mod_range: 5, mod_range_slide: 0, mod_width: 0.5, mod_width_slide: 0}</pre></h4><h3>  </h3><h3>Argument Documentation:</h3><ul>  <li> note:<ul>    <li> doc: Note to play. Either a MIDI number or a symbol representing a note. For example: 30, 52, :C, :C2, :Eb4, or :Ds3</li>    <li> default: 52</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> note_slide:<ul>    <li> doc: Amount of time (in seconds) for the note to change. A long slide value means that the note takes a long time to slide from the previous note to the new note. A slide of 0 means that the note instantly changes to the new note.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp:<ul>    <li> doc: The amplitude of the sound. Typically a value between 0 and 1. Higher amplitudes may be used, but won't make the sound louder, it will just reduce the quality of all the sounds currently being played (due to compression.)</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> pan:<ul>    <li> doc: Position of sound in stereo. With headphones on, this means how much of the sound is in the left ear, and how much is in the right ear. With a value of -1, the soundis completely in the left ear, a value of 0 puts the sound equally in both ears and a value of 1 puts the sound in the right ear. Values in between -1 and 1 move the sound accordingly.</li>    <li> default: 0</li>    <li> constraints: must be a value between -1 and 1 inclusively</li>    <li>May be changed whilst playing</li></ul>  <li> attack:<ul>    <li> doc: Amount of time (in seconds) for sound to reach full amplitude. A short attack (i.e. 0.01) makes the initial part of the sound very percussive like a sharp tap. A longer attack (i.e 1) fades the sound in gently. Full length of sound is attack + sustain + release.</li>    <li> default: 0.01</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> sustain:<ul>    <li> doc: Amount of time (in seconds) for sound to remain at full amplitude. Longer sustain values result in longer sounds. Full length of sound is attack + sustain + release.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> release:<ul>    <li> doc: Amount of time (in seconds) for sound to move from full amplitude to silent. A short release (i.e. 0.01) makes the final part of the sound very percussive (potentially resulting in a click). A longer release (i.e 1) fades the sound out gently. Full length of sound is attack + sustain + release.</li>    <li> default: 2</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> cutoff:<ul>    <li> doc: MIDI note representing the highest frequences allowed to be present in the sound. A low value like 30 makes the sound round and dull, a high value like 100 makes the sound buzzy and crispy.</li>    <li> default: 100</li>    <li> constraints: must be zero or greater,must be a value less than 130</li>    <li>May be changed whilst playing</li></ul>  <li> cutoff_slide:<ul>    <li> doc: Amount of time (in seconds) for the cutoff value to change. A long cutoff_slide value means that the cutoff takes a long time to slide from the previous value to the new value. A cutoff_slide of 0 means that the cutoff instantly changes to the new value.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> mod_rate:<ul>    <li> doc: Number of times per second that the note switches between the two notes.</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> mod_rate_slide:<ul>    <li> doc: Amount of time (in seconds) for the mod_rate value to change. A long mod_rate_slide value means that the mod_rate takes a long time to slide from the previous value to the new value. A mod_rate_slide of 0 means that the mod_rate instantly changes to the new value.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> mod_range:<ul>    <li> doc: The size of gap between modulation notes. A gap of 12 is one octave.</li>    <li> default: 5</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> mod_range_slide:<ul>    <li> doc: Amount of time (in seconds) for the mod_range value to change. A long mod_range_slide value means that the mod_range takes a long time to slide from the previous value to the new value. A mod_range_slide of 0 means that the mod_range instantly changes to the new value.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> mod_width:<ul>    <li> doc: The phase width of the modulation. Represents how even the gap between modulations is.</li>    <li> default: 0.5</li>    <li> constraints: must be a value between 0 and 1 exclusively</li>    <li>May be changed whilst playing</li></ul>  <li> mod_width_slide:<ul>    <li> doc: Amount of time (in seconds) for the mod_width value to change. A long mod_width_slide value means that the mod_width takes a long time to slide from the previous value to the new value. A mod_width_slide of 0 means that the mod_width instantly changes to the new value.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul></li></ul>"));
synthsNameList->addItem(synths_item_13);

QListWidgetItem *synths_item_14 = new QListWidgetItem("Simple Modualted Sine Wave");
synths_item_14->setData(32, QVariant("<h2> Simple Modualted Sine Wave</h2><h2><pre>use_synth :mod_sine_s</pre></h2><h4><pre>{note: 52, note_slide: 0, amp: 1, amp_slide: 0, pan: 0, pan_slide: 0, attack: 0.01, sustain: 0, release: 2, slide: 0, mod_rate: 1, mod_rate_slide: 0, mod_range: 5, mod_range_slide: 0, mod_width: 0.5, mod_width_slide: 0}</pre></h4><h3>  </h3><h3>Argument Documentation:</h3><ul>  <li> note:<ul>    <li> doc: Note to play. Either a MIDI number or a symbol representing a note. For example: 30, 52, :C, :C2, :Eb4, or :Ds3</li>    <li> default: 52</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> note_slide:<ul>    <li> doc: Amount of time (in seconds) for the note to change. A long slide value means that the note takes a long time to slide from the previous note to the new note. A slide of 0 means that the note instantly changes to the new note.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp:<ul>    <li> doc: The amplitude of the sound. Typically a value between 0 and 1. Higher amplitudes may be used, but won't make the sound louder, it will just reduce the quality of all the sounds currently being played (due to compression.)</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp_slide:<ul>    <li> doc: Amount of time (in seconds) for the amplitude (amp) to change. A long slide value means that the amp takes a long time to slide from the previous amplitude to the new amplitude. A slide of 0 means that the amplitude instantly changes to the new amplitude.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> pan:<ul>    <li> doc: Position of sound in stereo. With headphones on, this means how much of the sound is in the left ear, and how much is in the right ear. With a value of -1, the soundis completely in the left ear, a value of 0 puts the sound equally in both ears and a value of 1 puts the sound in the right ear. Values in between -1 and 1 move the sound accordingly.</li>    <li> default: 0</li>    <li> constraints: must be a value between -1 and 1 inclusively</li>    <li>May be changed whilst playing</li></ul>  <li> pan_slide:<ul>    <li> doc: Amount of time (in seconds) for the pan to change. A long slide value means that the pan takes a long time to slide from the previous pan position to the new pan position. A slide of 0 means that the pan instantly changes to the new pan position.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> attack:<ul>    <li> doc: Amount of time (in seconds) for sound to reach full amplitude. A short attack (i.e. 0.01) makes the initial part of the sound very percussive like a sharp tap. A longer attack (i.e 1) fades the sound in gently. Full length of sound is attack + sustain + release.</li>    <li> default: 0.01</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> sustain:<ul>    <li> doc: Amount of time (in seconds) for sound to remain at full amplitude. Longer sustain values result in longer sounds. Full length of sound is attack + sustain + release.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> release:<ul>    <li> doc: Amount of time (in seconds) for sound to move from full amplitude to silent. A short release (i.e. 0.01) makes the final part of the sound very percussive (potentially resulting in a click). A longer release (i.e 1) fades the sound out gently. Full length of sound is attack + sustain + release.</li>    <li> default: 2</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> slide:<ul>    <li> doc: write me</li>    <li> default: 0</li>    <li> constraints: none</li>    <li>Can not be changed once set</li></ul>  <li> mod_rate:<ul>    <li> doc: Number of times per second that the note switches between the two notes.</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> mod_rate_slide:<ul>    <li> doc: Amount of time (in seconds) for the mod_rate value to change. A long mod_rate_slide value means that the mod_rate takes a long time to slide from the previous value to the new value. A mod_rate_slide of 0 means that the mod_rate instantly changes to the new value.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> mod_range:<ul>    <li> doc: The size of gap between modulation notes. A gap of 12 is one octave.</li>    <li> default: 5</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> mod_range_slide:<ul>    <li> doc: Amount of time (in seconds) for the mod_range value to change. A long mod_range_slide value means that the mod_range takes a long time to slide from the previous value to the new value. A mod_range_slide of 0 means that the mod_range instantly changes to the new value.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> mod_width:<ul>    <li> doc: The phase width of the modulation. Represents how even the gap between modulations is.</li>    <li> default: 0.5</li>    <li> constraints: must be a value between 0 and 1 exclusively</li>    <li>May be changed whilst playing</li></ul>  <li> mod_width_slide:<ul>    <li> doc: Amount of time (in seconds) for the mod_width value to change. A long mod_width_slide value means that the mod_width takes a long time to slide from the previous value to the new value. A mod_width_slide of 0 means that the mod_width instantly changes to the new value.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul></li></ul>"));
synthsNameList->addItem(synths_item_14);

QListWidgetItem *synths_item_15 = new QListWidgetItem("Modulated Triangle Wave");
synths_item_15->setData(32, QVariant("<h2> Modulated Triangle Wave</h2><h2><pre>use_synth :mod_tri</pre></h2><h4><pre>{note: 52, note_slide: 0, amp: 1, amp_slide: 0, pan: 0, attack: 0.01, sustain: 0, release: 2, cutoff: 100, cutoff_slide: 0, mod_rate: 1, mod_rate_slide: 0, mod_range: 5, mod_range_slide: 0, mod_width: 0.5, mod_width_slide: 0}</pre></h4><h3>  </h3><h3>Argument Documentation:</h3><ul>  <li> note:<ul>    <li> doc: Note to play. Either a MIDI number or a symbol representing a note. For example: 30, 52, :C, :C2, :Eb4, or :Ds3</li>    <li> default: 52</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> note_slide:<ul>    <li> doc: Amount of time (in seconds) for the note to change. A long slide value means that the note takes a long time to slide from the previous note to the new note. A slide of 0 means that the note instantly changes to the new note.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp:<ul>    <li> doc: The amplitude of the sound. Typically a value between 0 and 1. Higher amplitudes may be used, but won't make the sound louder, it will just reduce the quality of all the sounds currently being played (due to compression.)</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp_slide:<ul>    <li> doc: Amount of time (in seconds) for the amplitude (amp) to change. A long slide value means that the amp takes a long time to slide from the previous amplitude to the new amplitude. A slide of 0 means that the amplitude instantly changes to the new amplitude.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> pan:<ul>    <li> doc: Position of sound in stereo. With headphones on, this means how much of the sound is in the left ear, and how much is in the right ear. With a value of -1, the soundis completely in the left ear, a value of 0 puts the sound equally in both ears and a value of 1 puts the sound in the right ear. Values in between -1 and 1 move the sound accordingly.</li>    <li> default: 0</li>    <li> constraints: must be a value between -1 and 1 inclusively</li>    <li>May be changed whilst playing</li></ul>  <li> attack:<ul>    <li> doc: Amount of time (in seconds) for sound to reach full amplitude. A short attack (i.e. 0.01) makes the initial part of the sound very percussive like a sharp tap. A longer attack (i.e 1) fades the sound in gently. Full length of sound is attack + sustain + release.</li>    <li> default: 0.01</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> sustain:<ul>    <li> doc: Amount of time (in seconds) for sound to remain at full amplitude. Longer sustain values result in longer sounds. Full length of sound is attack + sustain + release.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> release:<ul>    <li> doc: Amount of time (in seconds) for sound to move from full amplitude to silent. A short release (i.e. 0.01) makes the final part of the sound very percussive (potentially resulting in a click). A longer release (i.e 1) fades the sound out gently. Full length of sound is attack + sustain + release.</li>    <li> default: 2</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> cutoff:<ul>    <li> doc: MIDI note representing the highest frequences allowed to be present in the sound. A low value like 30 makes the sound round and dull, a high value like 100 makes the sound buzzy and crispy.</li>    <li> default: 100</li>    <li> constraints: must be zero or greater,must be a value less than 130</li>    <li>May be changed whilst playing</li></ul>  <li> cutoff_slide:<ul>    <li> doc: Amount of time (in seconds) for the cutoff value to change. A long cutoff_slide value means that the cutoff takes a long time to slide from the previous value to the new value. A cutoff_slide of 0 means that the cutoff instantly changes to the new value.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> mod_rate:<ul>    <li> doc: Number of times per second that the note switches between the two notes.</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> mod_rate_slide:<ul>    <li> doc: Amount of time (in seconds) for the mod_rate value to change. A long mod_rate_slide value means that the mod_rate takes a long time to slide from the previous value to the new value. A mod_rate_slide of 0 means that the mod_rate instantly changes to the new value.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> mod_range:<ul>    <li> doc: The size of gap between modulation notes. A gap of 12 is one octave.</li>    <li> default: 5</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> mod_range_slide:<ul>    <li> doc: Amount of time (in seconds) for the mod_range value to change. A long mod_range_slide value means that the mod_range takes a long time to slide from the previous value to the new value. A mod_range_slide of 0 means that the mod_range instantly changes to the new value.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> mod_width:<ul>    <li> doc: The phase width of the modulation. Represents how even the gap between modulations is.</li>    <li> default: 0.5</li>    <li> constraints: must be a value between 0 and 1 exclusively</li>    <li>May be changed whilst playing</li></ul>  <li> mod_width_slide:<ul>    <li> doc: Amount of time (in seconds) for the mod_width value to change. A long mod_width_slide value means that the mod_width takes a long time to slide from the previous value to the new value. A mod_width_slide of 0 means that the mod_width instantly changes to the new value.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul></li></ul>"));
synthsNameList->addItem(synths_item_15);

QListWidgetItem *synths_item_16 = new QListWidgetItem("Simple Modulated Triangle Wave");
synths_item_16->setData(32, QVariant("<h2> Simple Modulated Triangle Wave</h2><h2><pre>use_synth :mod_tri_s</pre></h2><h4><pre>{note: 52, note_slide: 0, amp: 1, pan: 0, attack: 0.01, sustain: 0, release: 2, slide: 0, mod_rate: 1, mod_range: 5, mod_width: 0.5}</pre></h4><h3>  </h3><h3>Argument Documentation:</h3><ul>  <li> note:<ul>    <li> doc: Note to play. Either a MIDI number or a symbol representing a note. For example: 30, 52, :C, :C2, :Eb4, or :Ds3</li>    <li> default: 52</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> note_slide:<ul>    <li> doc: Amount of time (in seconds) for the note to change. A long slide value means that the note takes a long time to slide from the previous note to the new note. A slide of 0 means that the note instantly changes to the new note.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp:<ul>    <li> doc: The amplitude of the sound. Typically a value between 0 and 1. Higher amplitudes may be used, but won't make the sound louder, it will just reduce the quality of all the sounds currently being played (due to compression.)</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> pan:<ul>    <li> doc: Position of sound in stereo. With headphones on, this means how much of the sound is in the left ear, and how much is in the right ear. With a value of -1, the soundis completely in the left ear, a value of 0 puts the sound equally in both ears and a value of 1 puts the sound in the right ear. Values in between -1 and 1 move the sound accordingly.</li>    <li> default: 0</li>    <li> constraints: must be a value between -1 and 1 inclusively</li>    <li>May be changed whilst playing</li></ul>  <li> attack:<ul>    <li> doc: Amount of time (in seconds) for sound to reach full amplitude. A short attack (i.e. 0.01) makes the initial part of the sound very percussive like a sharp tap. A longer attack (i.e 1) fades the sound in gently. Full length of sound is attack + sustain + release.</li>    <li> default: 0.01</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> sustain:<ul>    <li> doc: Amount of time (in seconds) for sound to remain at full amplitude. Longer sustain values result in longer sounds. Full length of sound is attack + sustain + release.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> release:<ul>    <li> doc: Amount of time (in seconds) for sound to move from full amplitude to silent. A short release (i.e. 0.01) makes the final part of the sound very percussive (potentially resulting in a click). A longer release (i.e 1) fades the sound out gently. Full length of sound is attack + sustain + release.</li>    <li> default: 2</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> slide:<ul>    <li> doc: write me</li>    <li> default: 0</li>    <li> constraints: none</li>    <li>Can not be changed once set</li></ul>  <li> mod_rate:<ul>    <li> doc: Number of times per second that the note switches between the two notes.</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> mod_range:<ul>    <li> doc: The size of gap between modulation notes. A gap of 12 is one octave.</li>    <li> default: 5</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> mod_width:<ul>    <li> doc: The phase width of the modulation. Represents how even the gap between modulations is.</li>    <li> default: 0.5</li>    <li> constraints: must be a value between 0 and 1 exclusively</li>    <li>May be changed whilst playing</li></ul></li></ul>"));
synthsNameList->addItem(synths_item_16);

QListWidgetItem *synths_item_17 = new QListWidgetItem("Modulated Pulse");
synths_item_17->setData(32, QVariant("<h2> Modulated Pulse</h2><h2><pre>use_synth :mod_pulse</pre></h2><h4><pre>{note: 52, note_slide: 0, amp: 1, amp_slide: 0, pan: 0, pan_slide: 0, attack: 0.01, sustain: 0, release: 2, cutoff: 100, cutoff_slide: 0, mod_rate: 1, mod_rate__slide: 0, mod_range: 5, mod_range_slide: 0, mod_width: 0.5, mod_width_slide: 0, pulse_width: 0.5, pulse_width_slide: 0}</pre></h4><h3>  </h3><h3>Argument Documentation:</h3><ul>  <li> note:<ul>    <li> doc: Note to play. Either a MIDI number or a symbol representing a note. For example: 30, 52, :C, :C2, :Eb4, or :Ds3</li>    <li> default: 52</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> note_slide:<ul>    <li> doc: Amount of time (in seconds) for the note to change. A long slide value means that the note takes a long time to slide from the previous note to the new note. A slide of 0 means that the note instantly changes to the new note.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp:<ul>    <li> doc: The amplitude of the sound. Typically a value between 0 and 1. Higher amplitudes may be used, but won't make the sound louder, it will just reduce the quality of all the sounds currently being played (due to compression.)</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp_slide:<ul>    <li> doc: Amount of time (in seconds) for the amplitude (amp) to change. A long slide value means that the amp takes a long time to slide from the previous amplitude to the new amplitude. A slide of 0 means that the amplitude instantly changes to the new amplitude.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> pan:<ul>    <li> doc: Position of sound in stereo. With headphones on, this means how much of the sound is in the left ear, and how much is in the right ear. With a value of -1, the soundis completely in the left ear, a value of 0 puts the sound equally in both ears and a value of 1 puts the sound in the right ear. Values in between -1 and 1 move the sound accordingly.</li>    <li> default: 0</li>    <li> constraints: must be a value between -1 and 1 inclusively</li>    <li>May be changed whilst playing</li></ul>  <li> pan_slide:<ul>    <li> doc: Amount of time (in seconds) for the pan to change. A long slide value means that the pan takes a long time to slide from the previous pan position to the new pan position. A slide of 0 means that the pan instantly changes to the new pan position.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> attack:<ul>    <li> doc: Amount of time (in seconds) for sound to reach full amplitude. A short attack (i.e. 0.01) makes the initial part of the sound very percussive like a sharp tap. A longer attack (i.e 1) fades the sound in gently. Full length of sound is attack + sustain + release.</li>    <li> default: 0.01</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> sustain:<ul>    <li> doc: Amount of time (in seconds) for sound to remain at full amplitude. Longer sustain values result in longer sounds. Full length of sound is attack + sustain + release.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> release:<ul>    <li> doc: Amount of time (in seconds) for sound to move from full amplitude to silent. A short release (i.e. 0.01) makes the final part of the sound very percussive (potentially resulting in a click). A longer release (i.e 1) fades the sound out gently. Full length of sound is attack + sustain + release.</li>    <li> default: 2</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> cutoff:<ul>    <li> doc: MIDI note representing the highest frequences allowed to be present in the sound. A low value like 30 makes the sound round and dull, a high value like 100 makes the sound buzzy and crispy.</li>    <li> default: 100</li>    <li> constraints: must be zero or greater,must be a value less than 130</li>    <li>May be changed whilst playing</li></ul>  <li> cutoff_slide:<ul>    <li> doc: Amount of time (in seconds) for the cutoff value to change. A long cutoff_slide value means that the cutoff takes a long time to slide from the previous value to the new value. A cutoff_slide of 0 means that the cutoff instantly changes to the new value.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> mod_rate:<ul>    <li> doc: Number of times per second that the note switches between the two notes.</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> mod_rate__slide:<ul>    <li> doc: write me</li>    <li> default: 0</li>    <li> constraints: none</li>    <li>Can not be changed once set</li></ul>  <li> mod_range:<ul>    <li> doc: The size of gap between modulation notes. A gap of 12 is one octave.</li>    <li> default: 5</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> mod_range_slide:<ul>    <li> doc: Amount of time (in seconds) for the mod_range value to change. A long mod_range_slide value means that the mod_range takes a long time to slide from the previous value to the new value. A mod_range_slide of 0 means that the mod_range instantly changes to the new value.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> mod_width:<ul>    <li> doc: The phase width of the modulation. Represents how even the gap between modulations is.</li>    <li> default: 0.5</li>    <li> constraints: must be a value between 0 and 1 exclusively</li>    <li>May be changed whilst playing</li></ul>  <li> mod_width_slide:<ul>    <li> doc: Amount of time (in seconds) for the mod_width value to change. A long mod_width_slide value means that the mod_width takes a long time to slide from the previous value to the new value. A mod_width_slide of 0 means that the mod_width instantly changes to the new value.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> pulse_width:<ul>    <li> doc: write me</li>    <li> default: 0.5</li>    <li> constraints: none</li>    <li>Can not be changed once set</li></ul>  <li> pulse_width_slide:<ul>    <li> doc: write me</li>    <li> default: 0</li>    <li> constraints: none</li>    <li>Can not be changed once set</li></ul></li></ul>"));
synthsNameList->addItem(synths_item_17);

QListWidgetItem *synths_item_18 = new QListWidgetItem("Simple Modulated Pulse");
synths_item_18->setData(32, QVariant("<h2> Simple Modulated Pulse</h2><h2><pre>use_synth :mod_pulse_s</pre></h2><h4><pre>{note: 52, note_slide: 0, amp: 1, pan: 0, attack: 0.01, sustain: 0, release: 2, mod_rate: 1, mod_range: 5, mod_width: 0.5, pulse_width: 0.5}</pre></h4><h3>  </h3><h3>Argument Documentation:</h3><ul>  <li> note:<ul>    <li> doc: Note to play. Either a MIDI number or a symbol representing a note. For example: 30, 52, :C, :C2, :Eb4, or :Ds3</li>    <li> default: 52</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> note_slide:<ul>    <li> doc: Amount of time (in seconds) for the note to change. A long slide value means that the note takes a long time to slide from the previous note to the new note. A slide of 0 means that the note instantly changes to the new note.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp:<ul>    <li> doc: The amplitude of the sound. Typically a value between 0 and 1. Higher amplitudes may be used, but won't make the sound louder, it will just reduce the quality of all the sounds currently being played (due to compression.)</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> pan:<ul>    <li> doc: Position of sound in stereo. With headphones on, this means how much of the sound is in the left ear, and how much is in the right ear. With a value of -1, the soundis completely in the left ear, a value of 0 puts the sound equally in both ears and a value of 1 puts the sound in the right ear. Values in between -1 and 1 move the sound accordingly.</li>    <li> default: 0</li>    <li> constraints: must be a value between -1 and 1 inclusively</li>    <li>May be changed whilst playing</li></ul>  <li> attack:<ul>    <li> doc: Amount of time (in seconds) for sound to reach full amplitude. A short attack (i.e. 0.01) makes the initial part of the sound very percussive like a sharp tap. A longer attack (i.e 1) fades the sound in gently. Full length of sound is attack + sustain + release.</li>    <li> default: 0.01</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> sustain:<ul>    <li> doc: Amount of time (in seconds) for sound to remain at full amplitude. Longer sustain values result in longer sounds. Full length of sound is attack + sustain + release.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> release:<ul>    <li> doc: Amount of time (in seconds) for sound to move from full amplitude to silent. A short release (i.e. 0.01) makes the final part of the sound very percussive (potentially resulting in a click). A longer release (i.e 1) fades the sound out gently. Full length of sound is attack + sustain + release.</li>    <li> default: 2</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> mod_rate:<ul>    <li> doc: Number of times per second that the note switches between the two notes.</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> mod_range:<ul>    <li> doc: The size of gap between modulation notes. A gap of 12 is one octave.</li>    <li> default: 5</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> mod_width:<ul>    <li> doc: The phase width of the modulation. Represents how even the gap between modulations is.</li>    <li> default: 0.5</li>    <li> constraints: must be a value between 0 and 1 exclusively</li>    <li>May be changed whilst playing</li></ul>  <li> pulse_width:<ul>    <li> doc: write me</li>    <li> default: 0.5</li>    <li> constraints: none</li>    <li>Can not be changed once set</li></ul></li></ul>"));
synthsNameList->addItem(synths_item_18);

QListWidgetItem *synths_item_19 = new QListWidgetItem("TB-303 Emulation");
synths_item_19->setData(32, QVariant("<h2> TB-303 Emulation</h2><h2><pre>use_synth :tb303</pre></h2><h4><pre>{note: 52, note_slide: 0, amp: 1, amp_slide: 0, pan: 0, pan_slide: 0, attack: 0.01, sustain: 0, release: 2, cutoff: 80, cutoff_slide: 0, cutoff_min: 30, res: 0.1, res_slide: 0, wave: 0, pulse_width: 0.5, pulse_width_slide: 0}</pre></h4><h3>  </h3><h3>Argument Documentation:</h3><ul>  <li> note:<ul>    <li> doc: Note to play. Either a MIDI number or a symbol representing a note. For example: 30, 52, :C, :C2, :Eb4, or :Ds3</li>    <li> default: 52</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> note_slide:<ul>    <li> doc: Amount of time (in seconds) for the note to change. A long slide value means that the note takes a long time to slide from the previous note to the new note. A slide of 0 means that the note instantly changes to the new note.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp:<ul>    <li> doc: The amplitude of the sound. Typically a value between 0 and 1. Higher amplitudes may be used, but won't make the sound louder, it will just reduce the quality of all the sounds currently being played (due to compression.)</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp_slide:<ul>    <li> doc: Amount of time (in seconds) for the amplitude (amp) to change. A long slide value means that the amp takes a long time to slide from the previous amplitude to the new amplitude. A slide of 0 means that the amplitude instantly changes to the new amplitude.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> pan:<ul>    <li> doc: Position of sound in stereo. With headphones on, this means how much of the sound is in the left ear, and how much is in the right ear. With a value of -1, the soundis completely in the left ear, a value of 0 puts the sound equally in both ears and a value of 1 puts the sound in the right ear. Values in between -1 and 1 move the sound accordingly.</li>    <li> default: 0</li>    <li> constraints: must be a value between -1 and 1 inclusively</li>    <li>May be changed whilst playing</li></ul>  <li> pan_slide:<ul>    <li> doc: Amount of time (in seconds) for the pan to change. A long slide value means that the pan takes a long time to slide from the previous pan position to the new pan position. A slide of 0 means that the pan instantly changes to the new pan position.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> attack:<ul>    <li> doc: Amount of time (in seconds) for sound to reach full amplitude. A short attack (i.e. 0.01) makes the initial part of the sound very percussive like a sharp tap. A longer attack (i.e 1) fades the sound in gently. Full length of sound is attack + sustain + release.</li>    <li> default: 0.01</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> sustain:<ul>    <li> doc: Amount of time (in seconds) for sound to remain at full amplitude. Longer sustain values result in longer sounds. Full length of sound is attack + sustain + release.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> release:<ul>    <li> doc: Amount of time (in seconds) for sound to move from full amplitude to silent. A short release (i.e. 0.01) makes the final part of the sound very percussive (potentially resulting in a click). A longer release (i.e 1) fades the sound out gently. Full length of sound is attack + sustain + release.</li>    <li> default: 2</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> cutoff:<ul>    <li> doc: </li>    <li> default: 80</li>    <li> constraints: must be zero or greater,must be a value less than 130</li>    <li>May be changed whilst playing</li></ul>  <li> cutoff_slide:<ul>    <li> doc: Amount of time (in seconds) for the cutoff value to change. A long cutoff_slide value means that the cutoff takes a long time to slide from the previous value to the new value. A cutoff_slide of 0 means that the cutoff instantly changes to the new value.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> cutoff_min:<ul>    <li> doc: </li>    <li> default: 30</li>    <li> constraints: must be zero or greater,must be a value less than 130</li>    <li>May be changed whilst playing</li></ul>  <li> res:<ul>    <li> doc: write me</li>    <li> default: 0.1</li>    <li> constraints: none</li>    <li>Can not be changed once set</li></ul>  <li> res_slide:<ul>    <li> doc: write me</li>    <li> default: 0</li>    <li> constraints: none</li>    <li>Can not be changed once set</li></ul>  <li> wave:<ul>    <li> doc: Wave type - 0 saw, 1 pulse</li>    <li> default: 0</li>    <li> constraints: must be one of the following values: [0, 1]</li>    <li>May be changed whilst playing</li></ul>  <li> pulse_width:<ul>    <li> doc: Only valid if wave is type pulse.</li>    <li> default: 0.5</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> pulse_width_slide:<ul>    <li> doc: Time in seconds for pulse width to change. Only valid if wave is type pulse.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul></li></ul>"));
synthsNameList->addItem(synths_item_19);

QListWidgetItem *synths_item_20 = new QListWidgetItem("Supersaw");
synths_item_20->setData(32, QVariant("<h2> Supersaw</h2><h2><pre>use_synth :supersaw</pre></h2><h4><pre>{note: 52, note_slide: 0, amp: 1, amp_slide: 0, pan: 0, pan_slide: 0, attack: 0.01, sustain: 0, release: 2, cutoff: 130, cutoff_slide: 0, res: 0.3, res_slide: 0}</pre></h4><h3>  </h3><h3>Argument Documentation:</h3><ul>  <li> note:<ul>    <li> doc: Note to play. Either a MIDI number or a symbol representing a note. For example: 30, 52, :C, :C2, :Eb4, or :Ds3</li>    <li> default: 52</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> note_slide:<ul>    <li> doc: Amount of time (in seconds) for the note to change. A long slide value means that the note takes a long time to slide from the previous note to the new note. A slide of 0 means that the note instantly changes to the new note.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp:<ul>    <li> doc: The amplitude of the sound. Typically a value between 0 and 1. Higher amplitudes may be used, but won't make the sound louder, it will just reduce the quality of all the sounds currently being played (due to compression.)</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp_slide:<ul>    <li> doc: Amount of time (in seconds) for the amplitude (amp) to change. A long slide value means that the amp takes a long time to slide from the previous amplitude to the new amplitude. A slide of 0 means that the amplitude instantly changes to the new amplitude.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> pan:<ul>    <li> doc: Position of sound in stereo. With headphones on, this means how much of the sound is in the left ear, and how much is in the right ear. With a value of -1, the soundis completely in the left ear, a value of 0 puts the sound equally in both ears and a value of 1 puts the sound in the right ear. Values in between -1 and 1 move the sound accordingly.</li>    <li> default: 0</li>    <li> constraints: must be a value between -1 and 1 inclusively</li>    <li>May be changed whilst playing</li></ul>  <li> pan_slide:<ul>    <li> doc: Amount of time (in seconds) for the pan to change. A long slide value means that the pan takes a long time to slide from the previous pan position to the new pan position. A slide of 0 means that the pan instantly changes to the new pan position.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> attack:<ul>    <li> doc: Amount of time (in seconds) for sound to reach full amplitude. A short attack (i.e. 0.01) makes the initial part of the sound very percussive like a sharp tap. A longer attack (i.e 1) fades the sound in gently. Full length of sound is attack + sustain + release.</li>    <li> default: 0.01</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> sustain:<ul>    <li> doc: Amount of time (in seconds) for sound to remain at full amplitude. Longer sustain values result in longer sounds. Full length of sound is attack + sustain + release.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> release:<ul>    <li> doc: Amount of time (in seconds) for sound to move from full amplitude to silent. A short release (i.e. 0.01) makes the final part of the sound very percussive (potentially resulting in a click). A longer release (i.e 1) fades the sound out gently. Full length of sound is attack + sustain + release.</li>    <li> default: 2</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> cutoff:<ul>    <li> doc: MIDI note representing the highest frequences allowed to be present in the sound. A low value like 30 makes the sound round and dull, a high value like 100 makes the sound buzzy and crispy.</li>    <li> default: 130</li>    <li> constraints: must be zero or greater,must be a value less than 130</li>    <li>May be changed whilst playing</li></ul>  <li> cutoff_slide:<ul>    <li> doc: Amount of time (in seconds) for the cutoff value to change. A long cutoff_slide value means that the cutoff takes a long time to slide from the previous value to the new value. A cutoff_slide of 0 means that the cutoff instantly changes to the new value.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> res:<ul>    <li> doc: write me</li>    <li> default: 0.3</li>    <li> constraints: none</li>    <li>Can not be changed once set</li></ul>  <li> res_slide:<ul>    <li> doc: write me</li>    <li> default: 0</li>    <li> constraints: none</li>    <li>Can not be changed once set</li></ul></li></ul>"));
synthsNameList->addItem(synths_item_20);

QListWidgetItem *synths_item_21 = new QListWidgetItem("Supersaw Simple");
synths_item_21->setData(32, QVariant("<h2> Supersaw Simple</h2><h2><pre>use_synth :supersaw_s</pre></h2><h4><pre>{note: 52, note_slide: 0, amp: 1, amp_slide: 0, pan: 0, pan_slide: 0, attack: 0.01, sustain: 0}</pre></h4><h3>  </h3><h3>Argument Documentation:</h3><ul>  <li> note:<ul>    <li> doc: Note to play. Either a MIDI number or a symbol representing a note. For example: 30, 52, :C, :C2, :Eb4, or :Ds3</li>    <li> default: 52</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> note_slide:<ul>    <li> doc: Amount of time (in seconds) for the note to change. A long slide value means that the note takes a long time to slide from the previous note to the new note. A slide of 0 means that the note instantly changes to the new note.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp:<ul>    <li> doc: The amplitude of the sound. Typically a value between 0 and 1. Higher amplitudes may be used, but won't make the sound louder, it will just reduce the quality of all the sounds currently being played (due to compression.)</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp_slide:<ul>    <li> doc: Amount of time (in seconds) for the amplitude (amp) to change. A long slide value means that the amp takes a long time to slide from the previous amplitude to the new amplitude. A slide of 0 means that the amplitude instantly changes to the new amplitude.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> pan:<ul>    <li> doc: Position of sound in stereo. With headphones on, this means how much of the sound is in the left ear, and how much is in the right ear. With a value of -1, the soundis completely in the left ear, a value of 0 puts the sound equally in both ears and a value of 1 puts the sound in the right ear. Values in between -1 and 1 move the sound accordingly.</li>    <li> default: 0</li>    <li> constraints: must be a value between -1 and 1 inclusively</li>    <li>May be changed whilst playing</li></ul>  <li> pan_slide:<ul>    <li> doc: Amount of time (in seconds) for the pan to change. A long slide value means that the pan takes a long time to slide from the previous pan position to the new pan position. A slide of 0 means that the pan instantly changes to the new pan position.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> attack:<ul>    <li> doc: Amount of time (in seconds) for sound to reach full amplitude. A short attack (i.e. 0.01) makes the initial part of the sound very percussive like a sharp tap. A longer attack (i.e 1) fades the sound in gently. Full length of sound is attack + sustain + release.</li>    <li> default: 0.01</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> sustain:<ul>    <li> doc: Amount of time (in seconds) for sound to remain at full amplitude. Longer sustain values result in longer sounds. Full length of sound is attack + sustain + release.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul></li></ul>"));
synthsNameList->addItem(synths_item_21);

QListWidgetItem *synths_item_22 = new QListWidgetItem("The Prophet");
synths_item_22->setData(32, QVariant("<h2> The Prophet</h2><h2><pre>use_synth :prophet</pre></h2><h4><pre>{note: 52, note_slide: 0, amp: 1, amp_slide: 0, pan: 0, pan_slide: 0, attack: 0.01, sustain: 0, release: 2, cutoff: 110, cutoff_slide: 0, res: 0.3, res_slide: 0}</pre></h4><h3>  Dark and swirly, this synth uses Pulse Width Modulation (PWM) to create a timbre which continually moves around. This effect is created using the pulse ugen which produces a variable width square wave. We then control the width of the pulses using a variety of LFOs - sin-osc and lf-tri in this case. We use a number of these LFO modulated pulse ugens with varying LFO type and rate (and phase in some cases to provide the LFO with a different starting point. We then mix all these pulses together to create a thick sound and then feed it through a resonant low pass filter (rlpf). For extra bass, one of the pulses is an octave lower (half the frequency) and its LFO has a little bit of randomisation thrown into its frequency component for that extra bit of variety.</h3><h3>Argument Documentation:</h3><ul>  <li> note:<ul>    <li> doc: Note to play. Either a MIDI number or a symbol representing a note. For example: 30, 52, :C, :C2, :Eb4, or :Ds3</li>    <li> default: 52</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> note_slide:<ul>    <li> doc: Amount of time (in seconds) for the note to change. A long slide value means that the note takes a long time to slide from the previous note to the new note. A slide of 0 means that the note instantly changes to the new note.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp:<ul>    <li> doc: The amplitude of the sound. Typically a value between 0 and 1. Higher amplitudes may be used, but won't make the sound louder, it will just reduce the quality of all the sounds currently being played (due to compression.)</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp_slide:<ul>    <li> doc: Amount of time (in seconds) for the amplitude (amp) to change. A long slide value means that the amp takes a long time to slide from the previous amplitude to the new amplitude. A slide of 0 means that the amplitude instantly changes to the new amplitude.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> pan:<ul>    <li> doc: Position of sound in stereo. With headphones on, this means how much of the sound is in the left ear, and how much is in the right ear. With a value of -1, the soundis completely in the left ear, a value of 0 puts the sound equally in both ears and a value of 1 puts the sound in the right ear. Values in between -1 and 1 move the sound accordingly.</li>    <li> default: 0</li>    <li> constraints: must be a value between -1 and 1 inclusively</li>    <li>May be changed whilst playing</li></ul>  <li> pan_slide:<ul>    <li> doc: Amount of time (in seconds) for the pan to change. A long slide value means that the pan takes a long time to slide from the previous pan position to the new pan position. A slide of 0 means that the pan instantly changes to the new pan position.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> attack:<ul>    <li> doc: Amount of time (in seconds) for sound to reach full amplitude. A short attack (i.e. 0.01) makes the initial part of the sound very percussive like a sharp tap. A longer attack (i.e 1) fades the sound in gently. Full length of sound is attack + sustain + release.</li>    <li> default: 0.01</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> sustain:<ul>    <li> doc: Amount of time (in seconds) for sound to remain at full amplitude. Longer sustain values result in longer sounds. Full length of sound is attack + sustain + release.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> release:<ul>    <li> doc: Amount of time (in seconds) for sound to move from full amplitude to silent. A short release (i.e. 0.01) makes the final part of the sound very percussive (potentially resulting in a click). A longer release (i.e 1) fades the sound out gently. Full length of sound is attack + sustain + release.</li>    <li> default: 2</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> cutoff:<ul>    <li> doc: MIDI note representing the highest frequences allowed to be present in the sound. A low value like 30 makes the sound round and dull, a high value like 100 makes the sound buzzy and crispy.</li>    <li> default: 110</li>    <li> constraints: must be zero or greater,must be a value less than 130</li>    <li>May be changed whilst playing</li></ul>  <li> cutoff_slide:<ul>    <li> doc: Amount of time (in seconds) for the cutoff value to change. A long cutoff_slide value means that the cutoff takes a long time to slide from the previous value to the new value. A cutoff_slide of 0 means that the cutoff instantly changes to the new value.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> res:<ul>    <li> doc: write me</li>    <li> default: 0.3</li>    <li> constraints: none</li>    <li>Can not be changed once set</li></ul>  <li> res_slide:<ul>    <li> doc: write me</li>    <li> default: 0</li>    <li> constraints: none</li>    <li>Can not be changed once set</li></ul></li></ul>"));
synthsNameList->addItem(synths_item_22);

QListWidgetItem *synths_item_23 = new QListWidgetItem("Zawa");
synths_item_23->setData(32, QVariant("<h2> Zawa</h2><h2><pre>use_synth :zawa</pre></h2><h4><pre>{note: 52, note_slide: 0, amp: 1, amp_slide: 0, pan: 0, pan_slide: 0, attack: 0.1, sustain: 0, release: 1, cutoff: 100, cutoff_slide: 0, rate: 1, rate_slide: 0, depth: 1.5, depth_slide: 0}</pre></h4><h3>  Write me</h3><h3>Argument Documentation:</h3><ul>  <li> note:<ul>    <li> doc: Note to play. Either a MIDI number or a symbol representing a note. For example: 30, 52, :C, :C2, :Eb4, or :Ds3</li>    <li> default: 52</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> note_slide:<ul>    <li> doc: Amount of time (in seconds) for the note to change. A long slide value means that the note takes a long time to slide from the previous note to the new note. A slide of 0 means that the note instantly changes to the new note.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp:<ul>    <li> doc: The amplitude of the sound. Typically a value between 0 and 1. Higher amplitudes may be used, but won't make the sound louder, it will just reduce the quality of all the sounds currently being played (due to compression.)</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp_slide:<ul>    <li> doc: Amount of time (in seconds) for the amplitude (amp) to change. A long slide value means that the amp takes a long time to slide from the previous amplitude to the new amplitude. A slide of 0 means that the amplitude instantly changes to the new amplitude.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> pan:<ul>    <li> doc: Position of sound in stereo. With headphones on, this means how much of the sound is in the left ear, and how much is in the right ear. With a value of -1, the soundis completely in the left ear, a value of 0 puts the sound equally in both ears and a value of 1 puts the sound in the right ear. Values in between -1 and 1 move the sound accordingly.</li>    <li> default: 0</li>    <li> constraints: must be a value between -1 and 1 inclusively</li>    <li>May be changed whilst playing</li></ul>  <li> pan_slide:<ul>    <li> doc: Amount of time (in seconds) for the pan to change. A long slide value means that the pan takes a long time to slide from the previous pan position to the new pan position. A slide of 0 means that the pan instantly changes to the new pan position.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> attack:<ul>    <li> doc: Amount of time (in seconds) for sound to reach full amplitude. A short attack (i.e. 0.01) makes the initial part of the sound very percussive like a sharp tap. A longer attack (i.e 1) fades the sound in gently. Full length of sound is attack + sustain + release.</li>    <li> default: 0.1</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> sustain:<ul>    <li> doc: Amount of time (in seconds) for sound to remain at full amplitude. Longer sustain values result in longer sounds. Full length of sound is attack + sustain + release.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> release:<ul>    <li> doc: Amount of time (in seconds) for sound to move from full amplitude to silent. A short release (i.e. 0.01) makes the final part of the sound very percussive (potentially resulting in a click). A longer release (i.e 1) fades the sound out gently. Full length of sound is attack + sustain + release.</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> cutoff:<ul>    <li> doc: MIDI note representing the highest frequences allowed to be present in the sound. A low value like 30 makes the sound round and dull, a high value like 100 makes the sound buzzy and crispy.</li>    <li> default: 100</li>    <li> constraints: must be zero or greater,must be a value less than 130</li>    <li>May be changed whilst playing</li></ul>  <li> cutoff_slide:<ul>    <li> doc: Amount of time (in seconds) for the cutoff value to change. A long cutoff_slide value means that the cutoff takes a long time to slide from the previous value to the new value. A cutoff_slide of 0 means that the cutoff instantly changes to the new value.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> rate:<ul>    <li> doc: write me</li>    <li> default: 1</li>    <li> constraints: none</li>    <li>Can not be changed once set</li></ul>  <li> rate_slide:<ul>    <li> doc: write me</li>    <li> default: 0</li>    <li> constraints: none</li>    <li>Can not be changed once set</li></ul>  <li> depth:<ul>    <li> doc: write me</li>    <li> default: 1.5</li>    <li> constraints: none</li>    <li>Can not be changed once set</li></ul>  <li> depth_slide:<ul>    <li> doc: write me</li>    <li> default: 0</li>    <li> constraints: none</li>    <li>Can not be changed once set</li></ul></li></ul>"));
synthsNameList->addItem(synths_item_23);

QListWidgetItem *synths_item_24 = new QListWidgetItem("Mono Sample Player");
synths_item_24->setData(32, QVariant("<h2> Mono Sample Player</h2><h2><pre>use_synth :mono_player</pre></h2><h4><pre>{amp: 1, amp_slide: 0, pan: 0, pan_slide: 0, attack: 0, sustain: -1, release: 0, rate: 1, start: 0, finish: 1}</pre></h4><h3>  </h3><h3>Argument Documentation:</h3><ul>  <li> amp:<ul>    <li> doc: The amplitude of the sound. Typically a value between 0 and 1. Higher amplitudes may be used, but won't make the sound louder, it will just reduce the quality of all the sounds currently being played (due to compression.)</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp_slide:<ul>    <li> doc: Amount of time (in seconds) for the amplitude (amp) to change. A long slide value means that the amp takes a long time to slide from the previous amplitude to the new amplitude. A slide of 0 means that the amplitude instantly changes to the new amplitude.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> pan:<ul>    <li> doc: Position of sound in stereo. With headphones on, this means how much of the sound is in the left ear, and how much is in the right ear. With a value of -1, the soundis completely in the left ear, a value of 0 puts the sound equally in both ears and a value of 1 puts the sound in the right ear. Values in between -1 and 1 move the sound accordingly.</li>    <li> default: 0</li>    <li> constraints: must be a value between -1 and 1 inclusively</li>    <li>May be changed whilst playing</li></ul>  <li> pan_slide:<ul>    <li> doc: Amount of time (in seconds) for the pan to change. A long slide value means that the pan takes a long time to slide from the previous pan position to the new pan position. A slide of 0 means that the pan instantly changes to the new pan position.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> attack:<ul>    <li> doc: </li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> sustain:<ul>    <li> doc: </li>    <li> default: -1</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> release:<ul>    <li> doc: </li>    <li> default: 0</li>    <li> constraints: must either be a positive value or -1</li>    <li>Can not be changed once set</li></ul>  <li> rate:<ul>    <li> doc: </li>    <li> default: 1</li>    <li> constraints: none</li>    <li>Can not be changed once set</li></ul>  <li> start:<ul>    <li> doc: </li>    <li> default: 0</li>    <li> constraints: must be zero or greater,must be a value between 0 and 1 inclusively</li>    <li>Can not be changed once set</li></ul>  <li> finish:<ul>    <li> doc: </li>    <li> default: 1</li>    <li> constraints: must be zero or greater,must be a value between 0 and 1 inclusively</li>    <li>Can not be changed once set</li></ul></li></ul>"));
synthsNameList->addItem(synths_item_24);

QListWidgetItem *synths_item_25 = new QListWidgetItem("Stereo Sample Player");
synths_item_25->setData(32, QVariant("<h2> Stereo Sample Player</h2><h2><pre>use_synth :stereo_player</pre></h2><h4><pre>{amp: 1, amp_slide: 0, pan: 0, pan_slide: 0, attack: 0, sustain: -1, release: 0, rate: 1, start: 0, finish: 1}</pre></h4><h3>  </h3><h3>Argument Documentation:</h3><ul>  <li> amp:<ul>    <li> doc: The amplitude of the sound. Typically a value between 0 and 1. Higher amplitudes may be used, but won't make the sound louder, it will just reduce the quality of all the sounds currently being played (due to compression.)</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp_slide:<ul>    <li> doc: Amount of time (in seconds) for the amplitude (amp) to change. A long slide value means that the amp takes a long time to slide from the previous amplitude to the new amplitude. A slide of 0 means that the amplitude instantly changes to the new amplitude.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> pan:<ul>    <li> doc: Position of sound in stereo. With headphones on, this means how much of the sound is in the left ear, and how much is in the right ear. With a value of -1, the soundis completely in the left ear, a value of 0 puts the sound equally in both ears and a value of 1 puts the sound in the right ear. Values in between -1 and 1 move the sound accordingly.</li>    <li> default: 0</li>    <li> constraints: must be a value between -1 and 1 inclusively</li>    <li>May be changed whilst playing</li></ul>  <li> pan_slide:<ul>    <li> doc: Amount of time (in seconds) for the pan to change. A long slide value means that the pan takes a long time to slide from the previous pan position to the new pan position. A slide of 0 means that the pan instantly changes to the new pan position.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> attack:<ul>    <li> doc: </li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> sustain:<ul>    <li> doc: </li>    <li> default: -1</li>    <li> constraints: must be zero or greater</li>    <li>Can not be changed once set</li></ul>  <li> release:<ul>    <li> doc: </li>    <li> default: 0</li>    <li> constraints: must either be a positive value or -1</li>    <li>Can not be changed once set</li></ul>  <li> rate:<ul>    <li> doc: </li>    <li> default: 1</li>    <li> constraints: none</li>    <li>Can not be changed once set</li></ul>  <li> start:<ul>    <li> doc: </li>    <li> default: 0</li>    <li> constraints: must be zero or greater,must be a value between 0 and 1 inclusively</li>    <li>Can not be changed once set</li></ul>  <li> finish:<ul>    <li> doc: </li>    <li> default: 1</li>    <li> constraints: must be zero or greater,must be a value between 0 and 1 inclusively</li>    <li>Can not be changed once set</li></ul></li></ul>"));
synthsNameList->addItem(synths_item_25);

QListWidgetItem *synths_item_26 = new QListWidgetItem("Basic Mono Sample Player - (no envelope)");
synths_item_26->setData(32, QVariant("<h2> Basic Mono Sample Player - (no envelope)</h2><h2><pre>use_synth :basic_mono_player</pre></h2><h4><pre>{amp: 1, amp_slide: 0, pan: 0, pan_slide: 0, rate: 1, rate_slide: 0}</pre></h4><h3>  </h3><h3>Argument Documentation:</h3><ul>  <li> amp:<ul>    <li> doc: The amplitude of the sound. Typically a value between 0 and 1. Higher amplitudes may be used, but won't make the sound louder, it will just reduce the quality of all the sounds currently being played (due to compression.)</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp_slide:<ul>    <li> doc: Amount of time (in seconds) for the amplitude (amp) to change. A long slide value means that the amp takes a long time to slide from the previous amplitude to the new amplitude. A slide of 0 means that the amplitude instantly changes to the new amplitude.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> pan:<ul>    <li> doc: Position of sound in stereo. With headphones on, this means how much of the sound is in the left ear, and how much is in the right ear. With a value of -1, the soundis completely in the left ear, a value of 0 puts the sound equally in both ears and a value of 1 puts the sound in the right ear. Values in between -1 and 1 move the sound accordingly.</li>    <li> default: 0</li>    <li> constraints: must be a value between -1 and 1 inclusively</li>    <li>May be changed whilst playing</li></ul>  <li> pan_slide:<ul>    <li> doc: Amount of time (in seconds) for the pan to change. A long slide value means that the pan takes a long time to slide from the previous pan position to the new pan position. A slide of 0 means that the pan instantly changes to the new pan position.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> rate:<ul>    <li> doc: write me</li>    <li> default: 1</li>    <li> constraints: none</li>    <li>Can not be changed once set</li></ul>  <li> rate_slide:<ul>    <li> doc: write me</li>    <li> default: 0</li>    <li> constraints: none</li>    <li>Can not be changed once set</li></ul></li></ul>"));
synthsNameList->addItem(synths_item_26);

QListWidgetItem *synths_item_27 = new QListWidgetItem("Basic Stereo Sample Player - (no envelope)");
synths_item_27->setData(32, QVariant("<h2> Basic Stereo Sample Player - (no envelope)</h2><h2><pre>use_synth :basic_stereo_player</pre></h2><h4><pre>{amp: 1, amp_slide: 0, pan: 0, pan_slide: 0, rate: 1, rate_slide: 0}</pre></h4><h3>  </h3><h3>Argument Documentation:</h3><ul>  <li> amp:<ul>    <li> doc: The amplitude of the sound. Typically a value between 0 and 1. Higher amplitudes may be used, but won't make the sound louder, it will just reduce the quality of all the sounds currently being played (due to compression.)</li>    <li> default: 1</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> amp_slide:<ul>    <li> doc: Amount of time (in seconds) for the amplitude (amp) to change. A long slide value means that the amp takes a long time to slide from the previous amplitude to the new amplitude. A slide of 0 means that the amplitude instantly changes to the new amplitude.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> pan:<ul>    <li> doc: Position of sound in stereo. With headphones on, this means how much of the sound is in the left ear, and how much is in the right ear. With a value of -1, the soundis completely in the left ear, a value of 0 puts the sound equally in both ears and a value of 1 puts the sound in the right ear. Values in between -1 and 1 move the sound accordingly.</li>    <li> default: 0</li>    <li> constraints: must be a value between -1 and 1 inclusively</li>    <li>May be changed whilst playing</li></ul>  <li> pan_slide:<ul>    <li> doc: Amount of time (in seconds) for the pan to change. A long slide value means that the pan takes a long time to slide from the previous pan position to the new pan position. A slide of 0 means that the pan instantly changes to the new pan position.</li>    <li> default: 0</li>    <li> constraints: must be zero or greater</li>    <li>May be changed whilst playing</li></ul>  <li> rate:<ul>    <li> doc: write me</li>    <li> default: 1</li>    <li> constraints: none</li>    <li>Can not be changed once set</li></ul>  <li> rate_slide:<ul>    <li> doc: write me</li>    <li> default: 0</li>    <li> constraints: none</li>    <li>Can not be changed once set</li></ul></li></ul>"));
synthsNameList->addItem(synths_item_27);

}
