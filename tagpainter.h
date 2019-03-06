#ifndef TAGPAINTER_H
#define TAGPAINTER_H

#include <QPainter>

class TagPainter
{
public:
  TagPainter(unsigned long start_id, int border, int corner_box,
             int padding, int cols, int rows, int unit_pixels);

  void PaintTagAt(QPainter &painter, unsigned long id, int x, int y);

  void PaintTagGrid(QPainter &painter);

  void PaintCornerGrid(QPainter &painter);

  void PaintTagBoard(QPainter& painter);

  int total_width() { return total_width_; }

  int total_height() { return total_height_; }

  int pixel_count_width() {
    return total_width_ * unit_pixels_ + 2 * padding_pixels_;
  }

  int pixel_count_height() {
    return total_height_ * unit_pixels_ + 2 * padding_pixels_;
  }

  int pixel_count_tag_size() { return tag_size_ * unit_pixels_; }

  int pixel_count_corner_box_size() { return corner_box_ * unit_pixels_; }

private:
  // input parameters
  unsigned long start_id_;
  int border_;
  int corner_box_;
  int padding_pixels_;
  int cols_;
  int rows_;
  int unit_pixels_;

  // calculated parameters
  int total_width_;
  int total_height_;
  int codes_size_;
  int tag_size_;
};

#endif // TAGPAINTER_H
