#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtSvg>
#include <QPdfWriter>


MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  validator = new QIntValidator(this);
  validator->setTop(1000);
  validator->setBottom(0);

  ui->lineEdit_ID         ->setValidator(validator);
  ui->lineEdit_Border     ->setValidator(validator);
  ui->lineEdit_CornerBox  ->setValidator(validator);
  ui->lineEdit_Cols       ->setValidator(validator);
  ui->lineEdit_Rows       ->setValidator(validator);
  ui->lineEdit_UnitPixels ->setValidator(validator);
  ui->lineEdit_Padding    ->setValidator(validator);
  ui->lineEdit_DPI        ->setValidator(validator);

  start_id_ = 0;
  border_ = 2;
  corner_box_ = 3;
  cols_ = 4;
  rows_ = 4;
  unit_pixels_ = 80;
  padding_pixels_ = 300;

  // for the convenience of converting inches to cm
  file_dpi_ = 254;

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
  ui->lineEdit_DPI        ->setText(QString::number(file_dpi_));
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
                           static_cast<int>(pixmap.height() * scale),
                           Qt::KeepAspectRatio, Qt::SmoothTransformation);
  }
  ui->label->setPixmap(pixmap);

  QString status =
      tr("tag_size: ")        + QString::number(pixel_to_cm(tp->pixel_count_tag_size())) +
      tr(" cm,  corner_box: ") + QString::number(pixel_to_cm(tp->pixel_count_corner_box_size())) +
      tr(" cm,  width: ")      + QString::number(pixel_to_cm(tp->pixel_count_width())) +
      tr(" cm,  height: ")     + QString::number(pixel_to_cm(tp->pixel_count_height())) +
      tr(" cm.\n") +
      tr("tag_size: ")        + QString::number(tp->pixel_count_tag_size()) +
      tr(" px,  corner_box: ") + QString::number(tp->pixel_count_corner_box_size()) +
      tr(" px,  width: ")      + QString::number(tp->pixel_count_width()) +
      tr(" px,  height: ")     + QString::number(tp->pixel_count_height()) +
      tr(" px.");
  ui->label_status->setText(status);

}

double MainWindow::pixel_to_cm(int pixel) {
  return pixel * 1.0 / file_dpi_ * 2.54;
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
  file_dpi_       = ui->lineEdit_DPI        ->text().toInt();

  paint_on_label();
}

void MainWindow::on_pushButtonSave_clicked()
{
  // save to file
  double width = pixel_to_cm(p_tag_painter_->pixel_count_width());
  double height = pixel_to_cm(p_tag_painter_->pixel_count_height());
  int first_id = static_cast<int>(start_id_);
  int last_id  = static_cast<int>(cols_ * rows_ + start_id_ - 1);
  char filename_cstr[80];
  std::sprintf(filename_cstr, "id_%d-%d_size_%.2fcmx%.2fcm.svg",
               first_id, last_id, width, height);
  path_ = QString::fromStdString(filename_cstr);

  QString new_path = QFileDialog::getSaveFileName(this, tr("Save to PDF or SVG"),
                                                  path_, tr("PDF (*.pdf);;SVG (*.svg)"));
  if (new_path.isEmpty()) {
    return;
  }

  enum {
    PDF = 0,
    SVG = 1,
  };

  int filetype = PDF;

  if (new_path.length() < 4) {
    path_ = new_path + ".pdf";
  } else {
    if (new_path.endsWith(".pdf", Qt::CaseInsensitive)) {
      path_ = new_path;
    } else if (new_path.endsWith(".svg", Qt::CaseInsensitive)) {
      path_ = new_path;
      filetype = SVG;
    } else {
      path_ = new_path + ".pdf";
    }
  }
  qDebug() << "path to save: " << path_;

  QString title = tr("AprilTags, ID from ") + QString::number(first_id) +
                  tr(" to ") + QString::number(last_id);

  if (filetype == SVG) {
    QSvgGenerator svg_generator;
    svg_generator.setFileName(path_);
    svg_generator.setSize(QSize(p_tag_painter_->pixel_count_width(),
                                p_tag_painter_->pixel_count_height()));
    svg_generator.setViewBox(QRect(0, 0,
                                   p_tag_painter_->pixel_count_width(),
                                   p_tag_painter_->pixel_count_height()));
    svg_generator.setTitle(title);
    svg_generator.setResolution(file_dpi_);
    QPainter painter(&svg_generator);
    p_tag_painter_->PaintTagBoard(painter);
  } else if (filetype == PDF) {
    QPdfWriter pdf_writer(path_);
    pdf_writer.setTitle(title);
    pdf_writer.setResolution(file_dpi_);
    QPageSize page_size(QSizeF(width * 10.0,
                               height * 10.0),
                        QPageSize::Millimeter, "", QPageSize::FuzzyMatch);
    pdf_writer.setPageSize(page_size);
    pdf_writer.setPageMargins(QMarginsF(0, 0, 0, 0));
    pdf_writer.newPage();
    QPainter painter(&pdf_writer);
    p_tag_painter_->PaintTagBoard(painter);
  }
}
