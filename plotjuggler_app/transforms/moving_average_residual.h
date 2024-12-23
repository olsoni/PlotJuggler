#pragma once

#include <QRadioButton>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include "PlotJuggler/transform_function.h"
#include "ui_moving_average_filter.h"
#include "PlotJuggler/contrib/ring_span.hpp"

using namespace PJ;

namespace Ui
{
class MovingAverageResidual;
}

class MovingAverageResidual : public TransformFunction_SISO
{
public:
  explicit MovingAverageResidual();

  ~MovingAverageResidual() override;

  void reset() override;

  static const char* transformName()
  {
    return "Moving Average Residual";
  }

  const char* name() const override
  {
    return transformName();
  }

  QWidget* optionsWidget() override;

  bool xmlSaveState(QDomDocument& doc, QDomElement& parent_element) const override;

  bool xmlLoadState(const QDomElement& parent_element) override;

private:
  Ui::MovingAverageResidual* ui;
  QWidget* _widget;
  std::vector<PlotData::Point> _buffer;
  nonstd::ring_span_lite::ring_span<PlotData::Point> _ring_view;

  std::optional<PlotData::Point> calculateNextPoint(size_t index) override;
};
