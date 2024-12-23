#include "moving_average_residual.h"
#include "ui_moving_average_residual.h"
#include <QCheckBox>

MovingAverageResidual::MovingAverageResidual()
  : ui(new Ui::MovingAverageResidual)
  , _widget(new QWidget())
  , _buffer(1)
  , _ring_view(_buffer.begin(), _buffer.end())
{
  ui->setupUi(_widget);

  connect(ui->spinBoxSamples, qOverload<int>(&QSpinBox::valueChanged), this,
          [=](int) { emit parametersChanged(); });

  connect(ui->checkBoxTimeOffset, &QCheckBox::toggled, this,
          [=]() { emit parametersChanged(); });
}

MovingAverageResidual::~MovingAverageResidual()
{
  delete ui;
  delete _widget;
}

void MovingAverageResidual::reset()
{
  _buffer.clear();
  TransformFunction_SISO::reset();
}

std::optional<PlotData::Point> MovingAverageResidual::calculateNextPoint(size_t index)
{
  size_t buffer_size =
      std::min(size_t(ui->spinBoxSamples->value()), size_t(dataSource()->size()));
  if (buffer_size != _buffer.size())
  {
    _buffer.resize(buffer_size);
    _ring_view = nonstd::ring_span<PlotData::Point>(_buffer.begin(), _buffer.end());
  }

  const auto& p = dataSource()->at(index);
  _ring_view.push_back(p);

  while (_ring_view.size() < buffer_size)
  {
    _ring_view.push_back(p);
  }

  double total = 0;

  for (size_t i = 0; i < buffer_size; i++)
  {
    total += _ring_view[i].y;
  }

  double time = p.x;
  double val  = p.y;
  if (ui->checkBoxTimeOffset->isChecked())
  {
    size_t mid = buffer_size / 2;
    time = _ring_view[mid].x;
    val  = _ring_view[mid].y;
  }

  PlotData::Point out = { time, val - total / _ring_view.size() };
  return out;
}

QWidget* MovingAverageResidual::optionsWidget()
{
  return _widget;
}

bool MovingAverageResidual::xmlSaveState(QDomDocument& doc,
                                       QDomElement& parent_element) const
{
  QDomElement widget_el = doc.createElement("options");
  widget_el.setAttribute("value", ui->spinBoxSamples->value());
  widget_el.setAttribute("compensate_offset",
                         ui->checkBoxTimeOffset->isChecked() ? "true" : "false");
  parent_element.appendChild(widget_el);
  return true;
}

bool MovingAverageResidual::xmlLoadState(const QDomElement& parent_element)
{
  QDomElement widget_el = parent_element.firstChildElement("options");
  if (widget_el.isNull())
  {
    return false;
  }

  ui->spinBoxSamples->setValue(widget_el.attribute("value").toInt());
  bool checked = widget_el.attribute("compensate_offset") == "true";
  ui->checkBoxTimeOffset->setChecked(checked);
  return true;
}
