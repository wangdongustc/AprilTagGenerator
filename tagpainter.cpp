#include "tagpainter.h"
// clang-format off
#include <AprilTags/TagDetector.h>
#include <AprilTags/Tag36h11.h>
#include <AprilTags/Tag36h9.h>
#include <AprilTags/Tag25h9.h>
#include <AprilTags/Tag25h7.h>
#include <AprilTags/Tag16h5.h>
// clang-format on
#include <stack>
#include <map>
static AprilTags::TagCodes tag_codes = AprilTags::tagCodes36h11;
static Qt::GlobalColor fore_color = Qt::white;
static Qt::GlobalColor back_color = Qt::black;
static QString font_family = "Monospace";
static std::map<TagFamilyName, AprilTags::TagCodes> family_codes {
  {TagFamilyName::TAG36H11, AprilTags::tagCodes36h11},
  {TagFamilyName::TAG36H9,  AprilTags::tagCodes36h9},
  {TagFamilyName::TAG25H9,  AprilTags::tagCodes25h9},
  {TagFamilyName::TAG25H7,  AprilTags::tagCodes25h7},
  {TagFamilyName::TAG16H5,  AprilTags::tagCodes16h5},
};
TagPainter::TagPainter(TagFamilyName tag_family, unsigned long start_id,
                       int border, int corner_box, int padding_pixels,
                       int cols, int rows, int unit_pixels) :
  start_id_(start_id), border_(border), corner_box_(corner_box),
  padding_pixels_(padding_pixels), cols_(cols), rows_(rows),
  unit_pixels_(unit_pixels)
{
  tag_codes = family_codes.at(tag_family);
  // the tagcode size
  codes_size_ = static_cast<int>(std::sqrt(tag_codes.bits));
  // calculate parameters
  tag_size_ = static_cast<int>(border_ * 2 + codes_size_);
  total_width_ = tag_size_ * cols_ + corner_box_ * (cols_ + 1);
  total_height_ = tag_size_ * rows_ + corner_box_ * (rows_ + 1);
}

void TagPainter::PaintTagAt(QPainter &painter, unsigned long id, int x, int y)
{
  painter.save();

  // painter.setClipRect(QRect(x, y, tag_size_, tag_size_));
  // draw the entire tag with black
  painter.translate(x, y);
  int real_tag_size = tag_size_ * unit_pixels_;
  painter.fillRect(QRect(0, 0, real_tag_size, real_tag_size), back_color);
  // draw the codes area with white
  painter.translate(border_ * unit_pixels_, border_ * unit_pixels_);
  int real_codes_size = codes_size_ * unit_pixels_;
  painter.fillRect(QRect(0, 0, real_codes_size, real_codes_size), fore_color);

  // paint according to the tag_id
  int i = 0;
  unsigned long long code = tag_codes.codes[id];
  std::stack<bool> code_stack;
  while (i != tag_codes.bits) {
    code_stack.push(code & 1);
    i++;
    code >>= 1;
  }
  for (int col = 0; col < codes_size_; col++) {
    for (int row = 0; row < codes_size_; row++) {
      // fill the pixel with while if the code bit is true
      painter.fillRect(QRect(row * unit_pixels_, col * unit_pixels_,
                             unit_pixels_, unit_pixels_),
                             code_stack.top() ? fore_color : back_color);
      code_stack.pop();
    }
  }

  painter.restore();
}

void TagPainter::PaintTagGrid(QPainter &painter)
{
  painter.save();

  // paint the entire working rectangle
  painter.fillRect(QRect(0, 0,
                         total_width_ * unit_pixels_ + padding_pixels_ * 2,
                         total_height_ * unit_pixels_ + padding_pixels_ * 2),
                   fore_color);
  // paint the tags from the start id to the end
  auto id = start_id_;
  for (int j = rows_ - 1; j >= 0; --j) {
    for (int i = 0; i < cols_; ++i) {
      int x = padding_pixels_ +
              unit_pixels_ * ((i + 1) * corner_box_ + i * tag_size_);
      int y = padding_pixels_ +
              unit_pixels_ * ((j + 1) * corner_box_ + j * tag_size_);
      PaintTagAt(painter, id, x, y);
      id++;
    }
  }

  painter.restore();
}

void TagPainter::PaintCornerGrid(QPainter &painter)
{
  painter.save();

  // paint the corner box grids
  for (int i = 0; i < cols_ + 1; ++i) {
    for (int j = 0; j < rows_ + 1; ++j) {
      int x = padding_pixels_ + unit_pixels_ * (i * (corner_box_ + tag_size_));
      int y = padding_pixels_ + unit_pixels_ * (j * (corner_box_ + tag_size_));
      painter.fillRect(QRect(x, y,
                             corner_box_ * unit_pixels_,
                             corner_box_ * unit_pixels_),
                       back_color);
    }
  }
  painter.restore();
}

void TagPainter::PaintTagBoard(QPainter &painter)
{
  PaintTagGrid(painter);
  PaintCornerGrid(painter);
  // if you want to make a print acrylic board, here is an example for
  // add drillhole guides on the board if you want the manufactor to
  // drill holes on the board.
  // PaintDrillHoles(painter);
  PaintLabel(painter);
  PaintCorrdinate(painter);
}

void TagPainter::PaintDrillHoles(QPainter& painter) {
  painter.save();

  int hole_r = 1;
  int hole_d = 2 * hole_r;
  painter.setPen(Qt::red);
  // painter.setBrush(Qt::black);
  for (int i = 0; i < 4; ++i) {
    painter.drawEllipse(200 + i * 200 - hole_r, 25 - hole_r,            hole_d, hole_d);
    painter.drawEllipse(200 + i * 200 - hole_r, 1000 - 25 - hole_r,     hole_d, hole_d);
    painter.drawEllipse(25 - hole_r,            200 + i * 200 - hole_r, hole_d, hole_d);
    painter.drawEllipse(1000 - 25 - hole_r,     200 + i * 200 - hole_r, hole_d, hole_d);
  }
  for (int i = 0; i < 3; ++i) {
    painter.drawEllipse(300 + i * 200 - hole_r, 50 - hole_r,            hole_d, hole_d);
    painter.drawEllipse(300 + i * 200 - hole_r, 1000 - 50 - hole_r,     hole_d, hole_d);
    painter.drawEllipse(50 - hole_r,            300 + i * 200 - hole_r, hole_d, hole_d);
    painter.drawEllipse(1000 - 50 - hole_r,     300 + i * 200 - hole_r, hole_d, hole_d);
  }

  painter.restore();
}

void TagPainter::PaintLabel(QPainter &painter) {
  painter.save();
  int font_size = padding_pixels_ / 2;
  QFont font;
  font.setFamily(font_family);
  font.setPixelSize(font_size);
  painter.setFont(font);
  // to the origin
  painter.translate(0.5 * padding_pixels_,
                    total_height_ * unit_pixels_ + 1.5 * padding_pixels_);
  // just behind the text "x"
  double x_label_end = static_cast<int>(unit_pixels_ * tag_size_  / 2.0 * 1.1);
  int end_id = static_cast<int>(start_id_) + cols_ * rows_ - 1;
  painter.drawText(QPointF(x_label_end + font_size, font_size / 2),
                   QString::fromStdString(
                     "ID:" + std::to_string(start_id_) +
                     "-" + std::to_string(end_id))
                   );
  painter.restore();
}

void TagPainter::PaintCorrdinate(QPainter& painter) {
  painter.save();
  auto color_x = Qt::red;
  auto color_y = Qt::blue;
  int linewidth = std::max(unit_pixels_ / 4, 4);
  double arraw_size = unit_pixels_;
  double arraw_len = unit_pixels_ * tag_size_ / 2.0;
  double angle = 12.0 * 3.14159 / 180.0;
  double arraw_width = arraw_size * std::tan(angle);
  QPen pen;
  QBrush brush;
  pen.setWidth(linewidth);
  brush.setStyle(Qt::BrushStyle::SolidPattern);
  QFont font;
  int font_size = padding_pixels_ / 2;
  font.setFamily(font_family);
  font.setPixelSize(font_size);
  painter.setFont(font);

  // to the origin
  painter.translate(0.5 * padding_pixels_,
                    total_height_ * unit_pixels_ + 1.5 * padding_pixels_);

  // x-axis
  pen.setColor(color_x);
  brush.setColor(color_x);
  painter.setPen(pen);
  painter.setBrush(brush);

  painter.drawLine(0, 0, static_cast<int>(arraw_len), 0);
  std::vector<QPointF> x_arraw {
    {arraw_len, 0.0},
    {arraw_len - arraw_size, arraw_width},
    {arraw_len - arraw_size, -arraw_width},
  };
  painter.drawPolygon(x_arraw.data(), static_cast<int>(x_arraw.size()));
  painter.drawText(QPointF(arraw_len * 1.1, font_size / 2.0), "x");

  // y-axis
  pen.setColor(color_y);
  brush.setColor(color_y);
  painter.setPen(pen);
  painter.setBrush(brush);

  painter.drawLine(0, 0, 0, static_cast<int>(-arraw_len));
  std::vector<QPointF> y_arraw {
    {0.0, -arraw_len},
    {arraw_width, -arraw_len + arraw_size},
    {-arraw_width, -arraw_len + arraw_size},
  };
  painter.drawPolygon(y_arraw.data(), static_cast<int>(y_arraw.size()));
  painter.drawText(QPointF(-font_size / 2.0, -arraw_len * 1.1), "y");

  painter.restore();
}

