#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtSvg>


MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  validator = new QIntValidator(this);
  validator->setTop(100);
  validator->setBottom(0);

  ui->lineEdit_ID         ->setValidator(validator);
  ui->lineEdit_Border     ->setValidator(validator);
  ui->lineEdit_CornerBox  ->setValidator(validator);
  ui->lineEdit_Cols       ->setValidator(validator);
  ui->lineEdit_Rows       ->setValidator(validator);
  ui->lineEdit_UnitPixels ->setValidator(validator);
  ui->lineEdit_Padding    ->setValidator(validator);

  start_id_ = 0;
  border_ = 1;
  corner_box_ = 3;
  cols_ = 3;
  rows_ = 3;
  unit_pixels_ = 5;
  padding_pixels_ = 3 * unit_pixels_;

  reset_params();

  paint_on_label();
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::reset_params()
{
  ui->lineEdit_ID         ->setText(QString::number(start_id_));
  ui->lineEdit_Border     ->setText(QString::number(border_));
  ui->lineEdit_CornerBox  ->setText(QString::number(corner_box_));
  ui->lineEdit_Cols       ->setText(QString::number(cols_));
  ui->lineEdit_Rows       ->setText(QString::number(rows_));
  ui->lineEdit_UnitPixels ->setText(QString::number(unit_pixels_));
  ui->lineEdit_Padding    ->setText(QString::number(padding_pixels_));
}

void MainWindow::paint_on_label()
{
  p_tag_painter_ = std::make_shared<TagPainter>(start_id_, border_,
                                                corner_box_, padding_pixels_,
                                                cols_, rows_, unit_pixels_);
  auto& tp = p_tag_painter_;

  QPixmap pixmap(tp->pixel_count_width(), tp->pixel_count_height());
  {
    QPainter painter(&pixmap);
    tp->PaintTagBoard(painter);
  }

  // scale the image if needed
  double ctrl_size = 800;
  double scale = ctrl_size / static_cast<double>(std::max(pixmap.width(), pixmap.height()));
  if (scale < 1) {
    // image size is larger than controlled size
    pixmap = pixmap.scaled(static_cast<int>(pixmap.width() * scale),
                           static_cast<int>(pixmap.height() * scale));
  }
  ui->label->setPixmap(pixmap);

  QString status =
      tr("tag_size: ")     + QString::number(tp->pixel_count_tag_size()) +
      tr(",  corner_box: ") + QString::number(tp->pixel_count_corner_box_size()) +
      tr(",  width: ")      + QString::number(tp->pixel_count_width()) +
      tr(",  height: ")     + QString::number(tp->pixel_count_height());
  ui->label_status->setText(status);

}

void MainWindow::on_pushButtonClose_clicked()
{
  close();
}

void MainWindow::on_pushButtonReset_clicked()
{
  reset_params();
  paint_on_label();
}

void MainWindow::on_pushButtonApply_clicked()
{
  start_id_       = ui->lineEdit_ID         ->text().toULongLong();
  border_         = ui->lineEdit_Border     ->text().toInt();
  corner_box_     = ui->lineEdit_CornerBox  ->text().toInt();
  cols_           = ui->lineEdit_Cols       ->text().toInt();
  rows_           = ui->lineEdit_Rows       ->text().toInt();
  unit_pixels_    = ui->lineEdit_UnitPixels ->text().toInt();
  padding_pixels_ = ui->lineEdit_Padding    ->text().toInt();

  paint_on_label();
}

void MainWindow::on_pushButtonSave_clicked()
{
  // save to file
  QString new_path = QFileDialog::getSaveFileName(this, tr("Save SVG"),
                                                  path_, tr("SVG files (*svg)"));
  if (new_path.isEmpty()) {
    return;
  }

  path_ = new_path;

  QSvgGenerator svg_generator;
  svg_generator.setFileName(path_);
  svg_generator.setSize(QSize(p_tag_painter_->total_width(),
                              p_tag_painter_->total_height()));
  svg_generator.setViewBox(QRect(0, 0,
                                 p_tag_painter_->total_width(),
                                 p_tag_painter_->total_height()));
  svg_generator.setTitle(tr("AprilTags, ID from ") +
                         QString::number(start_id_));
  svg_generator.setDescription(tr(""));
  QPainter painter(&svg_generator);
  p_tag_painter_->PaintTagBoard(painter);
}
