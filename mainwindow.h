#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QButtonGroup>
#include <QValidator>
#include <memory>

#include "tagpainter.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:

  void on_pushButtonClose_clicked();

  void on_pushButtonReset_clicked();

  void on_pushButtonApply_clicked();

  void on_pushButtonSave_clicked();

private:
  Ui::MainWindow *ui;
  QIntValidator* validator;

  unsigned long long start_id_;
  int border_;
  int corner_box_;
  int cols_;
  int rows_;
  int unit_pixels_;
  int padding_pixels_;
  int file_dpi_;
  TagFamilyName tag_family_;

  std::shared_ptr<TagPainter> p_tag_painter_;

  QString path_;

  void reset_params();
  void paint_on_label();
  inline double pixel_to_cm(int pixel);
};

#endif // MAINWINDOW_H
