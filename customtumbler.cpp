#include "customtumbler.h"
#include <qDebug>
#include <QEvent>
#include <QPolygon>
#include <math.h>
#include <QInputEvent>
#include <QKeyEvent>


CustomTumbler::CustomTumbler(QWidget *parent)
{
  //Установка геометрии
  setFocusPolicy(Qt::StrongFocus);
  setGeometry(0,0,125,150);
  setMinimumSize(125,125);

  //Дефолтная запись листа строк
  QStringList strList;
  for(int i = 0; i < 16; i++)
    strList << QString("%1").arg(i);
  setDefaultValues(0, 340, strList, 0);

  //Установка стиля отрисовки
  setStyle(CustomTumbler::TYPE_STYLE::SnowWhite);

  //Установка дефолтных параметров
  auto value = std::min(this->width()/2, this->height()/2)*0.6;
  setRadius(value);
  QFont font;
  m_font.setFamily("Courier");
  m_font.setPointSize(getRadius()*0.175);
  m_font.setWeight(20);
  m_font.setBold(true);

}
/// функция установки значений по умолчания для компонента CustomTumbler
/// startAngle - начальный угол поворота
/// endAngle   - конечный угол поворота
/// labels     - список надписей для компонента (от размера labels зависит количество
/// позиций для перключения)
/// currentPosition - текущая позиция для перключения
void CustomTumbler::setDefaultValues(qreal startAngle, qreal endAngle, QStringList &labels, int currentPosition)
{
  m_vect.clear();
  // Установка диапазона углов
  setStartAngle(startAngle);
  setEndAngle(endAngle);
  // Приведение к диапазону 0..360
  correctNegativeAngles();
  // Заполение надписей для labels
  setLabels(labels);
  // Проверка на валидный список подписей
  // Проверяется хотя бы по одной записи внутри списка
  if(!getLabels().isEmpty())
  {
    int PosAmount = getLabels().size();
    setPosAmount(PosAmount);
    // Проверка что позиция по умолчанию лежит в пределах надписей
  }
  else{
    // Создается одна пустая надпись и переключатель ставится в эту позицию
    QString emptyPos("");
    QStringList lst;
    lst << emptyPos;
    setLabels(lst);
    setCurrentPos(0);
    int sz = getLabels().size();
    setPosAmount(sz);
  }
  qreal tempAngle;
  // Расчет диапазона в случае если надписей > 1
  if(getPosAmount()-1 > 0)
    tempAngle = (getEndAngle()-getStartAngle())/(getPosAmount()-1);
  else
    tempAngle = getEndAngle() - getStartAngle();

  // Запослнение массива пар[i] -> {(QString - Надпись, qreal - Значение угла)}
  if(!getLabels().isEmpty())
  {
    m_vect.push_back(std::make_pair(getLabels().at(0), getStartAngle()));
    for(int i = 1; i < getPosAmount()-1;i++)
      m_vect.push_back(std::make_pair(getLabels().at(i), getStartAngle()+i*tempAngle));
    if(m_vect.size() < getLabels().size())
      m_vect.push_back(std::make_pair(*(getLabels().end()-1),getEndAngle()));
  }
  // Установка текущей позиции
  if(currentPosition>= 0 && currentPosition < getLabels().size())
    setCurrentPos(currentPosition);
  else
    setCurrentPos(0);
  m_currentAngle = m_vect.at(getCurrentPos()).second;
  // Получение общего количество позиций
  m_PosAmount = static_cast<unsigned>(getLabels().size());
}

void CustomTumbler::setStyle(unsigned int type)
{
    //Установка тёмного стиля
    if(type == TYPE_STYLE::Dracula)
    {
        Colors.textCl       = Styles::Dracula::GrayWhite;
        Colors.posCl        = Styles::Dracula::Red;
        Colors.borderCl     = Styles::Dracula::Green;
        Colors.selectedCl   = Styles::Dracula::Green;
        Colors.posBorderCl  = Styles::Dracula::GrayWhite;
        Colors.centralObjCl = Styles::Dracula::DarkGray;
        Colors.switcherCl   = Styles::Dracula::Asphalt;
        Colors.backGroundCl = Styles::Dracula::Asphalt;
    }
    //Установка светлого стиля
    else if(type ==TYPE_STYLE::SnowWhite)
    {
        Colors.textCl       = Styles::Light::DarkGreen;
        Colors.posCl        = Styles::Light::Orange;
        Colors.borderCl     = Styles::Light::Cyan;
        Colors.selectedCl   = Styles::Light::Cyan;
        Colors.posBorderCl  = Styles::Light::Gray;
        Colors.centralObjCl = Styles::Light::White;
        Colors.switcherCl   = Styles::Light::LightGray;
        Colors.backGroundCl = Styles::Light::LightGray;
    }
    setStyleSheet("* {background-color: " + QString(Colors.backGroundCl.name()) + "}");

}

void CustomTumbler::correctNegativeAngles()
{
    // Корректируются углы к диапазону [0..360]
    if(fmod(m_startAngle,360) < 0)
        m_startAngle = fmod(m_startAngle,360) + 360;
    if(fmod(m_endAngle,360) < 0)
        m_endAngle = fmod(m_endAngle, 360) + 360;
}

void CustomTumbler::calcPos(QMouseEvent* event)
{
    auto diffX = -(event->x()-CenterX);
    auto diffY = -(event->y()-CenterY);

    qreal angleStep = (getEndAngle()-getStartAngle())/getPosAmount();

    if(diffX >= -0.01 && diffX <= 0.01)
        diffX = 0.01;
    if(diffY >= -0.01 && diffY <= 0.01)
        diffY = 0.01;

    auto rad = std::atan(diffY/diffX);
    auto deg = rad*180/M_PI;
    // Корректирование углов в диапазоне от [0,360] - GRAD
    if(diffX < 0 && diffY  > 0)
        deg += 180;
    else if(diffX < 0 && diffY < 0)
        deg += 180;
    else if(diffX > 0 && diffY < 0)
        deg += 360;

    for(int i = 0; i < m_vect.size(); i++)
    {
        // Учет если начальный диапазон отрицательный и угол нажатия лежит в этом диапазоне
        if(getStartAngle()-angleStep < 0 && deg >= 360 + (getStartAngle()-angleStep))
        {
            if(getCurrentPos()!= i)
            {
                m_currentAngle = m_vect.at(i).second;
        setCurrentPos(i);
        repaint();
      }
      return;
    }
    // Учет когда всё лежит в пределах от startAngle-angleStep/2 до endAngle+angleStep/2
    // и нет отрицательных значений
    if(deg >= getStartAngle()-angleStep/2 && deg <= getEndAngle()+angleStep/2)
    {

      if(m_vect.at(i).second <= deg + angleStep/2 &&  m_vect.at(i).second >= deg - angleStep/2)
      {
        if(getCurrentPos()!= i)
        {
          m_currentAngle = m_vect.at(i).second;
          setCurrentPos(i);
          repaint();
        }
        return;
      }
    }
  }
}

void CustomTumbler::mousePressEvent(QMouseEvent *event)
{
  if(event->button() == Qt::MouseButton::LeftButton)
  {
    setPressed(true);
    calcPos(event);
  }

}

void CustomTumbler::mouseReleaseEvent(QMouseEvent *event)
{
  setPressed(false);
}

void CustomTumbler::mouseMoveEvent(QMouseEvent *event)
{
  if(isPressed())
    calcPos(event);
}

void CustomTumbler::keyPressEvent(QKeyEvent *_event)
{
  if(_event->key() == Qt::Key::Key_Right)
  {
    if(getCurrentPos() < m_vect.size()-1)
    {
      setCurrentPos(getCurrentPos()+1);
      m_currentAngle = m_vect.at(getCurrentPos()).second;
      CustomTumbler::repaint();
    }
    else if (getCurrentPos() >= m_vect.size()-1)
    {
      setCurrentPos(m_vect.size()-1);
    }
  }
  else if (_event->key() == Qt::Key::Key_Left)
  {
    if(getCurrentPos() > 0)
    {
      setCurrentPos(getCurrentPos()-1);
      m_currentAngle = m_vect.at(getCurrentPos()).second;
      CustomTumbler::repaint();
    }
    else if (m_currentPos <= 0)
      setCurrentPos(0);
  }
}

void CustomTumbler::wheelEvent(QWheelEvent *_event)
{
  if(_event->delta() > 0)
  {
    setCurrentPos(getCurrentPos()+1);

    if(getCurrentPos() > m_vect.size()-1)
      setCurrentPos(0);

    m_currentAngle = m_vect.at(m_currentPos).second;
    repaint();
  }
  else
  {
    m_currentPos--;
    if(m_currentPos >= 0)
    {
      m_currentAngle = m_vect.at(m_currentPos).second;
      repaint();
    }
    else{
      m_currentPos = 0;
    }
  }

}

void CustomTumbler::paintEvent(QPaintEvent *event)
{
  QPainter paint(this);

  paint.setRenderHint(QPainter::Antialiasing, true);
  QColor color(Colors.borderCl);

  paint.setPen(color);
  paint.setBrush(QColor(Colors.centralObjCl));
    paint.drawEllipse(QPointF(CenterX, CenterY),getRadius()/1.4,getRadius()/1.4);

  paint.setBrush(Qt::transparent);

  QVector<qreal> angles;

  QVector<float> posAngles;
  posAngles.push_back(getStartAngle());
  float temp = (getEndAngle()-getStartAngle())/(getPosAmount()-1);

  for(int i = 1; i < getPosAmount()-1;i++)
  {
    posAngles.push_back(getStartAngle()+i*temp);
  }
  posAngles.push_back(getEndAngle());

  QFontMetrics metrics(m_font);
  QStringList strList = getLabels();

  QStringList tempList = getLabels();
  qreal tempWidth;
  qreal tempHeigth;
  qreal RectX;
  qreal RectY;

  int maxwidth = 0;
  int symbols  = 0;
  // Поиск ширины надписей с заданным шрифтом
  // И одно временно поиск самой длинной строки
  for(int i = 0; i < getPosAmount();i++)
  {
    if(strList[i].size() > symbols)
      symbols = strList[i].size();
    if(metrics.width(strList[i]) > maxwidth)
      maxwidth = metrics.width(strList[i]);
  }
  //Ограничение для строк больше 5 символов
  if(symbols >= 5)
    maxwidth/=1.7;


  // Отрисовываются положения переключения и надписи к ним
  for(int i = 0; i < getPosAmount(); i++)
  {
    if(getCurrentPos() == i)
    {
      paint.setBrush(QColor(Colors.selectedCl));
      paint.setPen(QColor(Colors.posBorderCl));
      paint.drawEllipse(QPointF(CenterX-getRadius()*std::cos((m_vect.at(i).second)*M_PI/180),
                                CenterY-getRadius()*std::sin((m_vect.at(i).second)*M_PI/180)),
                        getRadius()/8,getRadius()/8);
    }
    else
    {
      paint.setBrush(QColor(Colors.posCl));
      paint.setPen(QColor(Colors.posBorderCl));
      paint.drawEllipse(QPointF(CenterX-getRadius()*std::cos((m_vect.at(i).second)*M_PI/180),
                                CenterY-getRadius()*std::sin((m_vect.at(i).second)*M_PI/180)),
                        getRadius()/12,getRadius()/12);
    }

    tempWidth = metrics.width(tempList[i]);
    tempHeigth = metrics.height();

    RectX      = CenterX-std::cos(m_vect.at(i).second*M_PI/180)*(getRadius()+maxwidth);/* - tempWidth - getRadius()/8;*/
    RectY      = CenterY-std::sin(m_vect.at(i).second*M_PI/180)*(getRadius()+maxwidth);/* - tempHeigth/2;*/

    // Установка прозрачности для вспомогательных объектов
    paint.setPen(Qt::transparent);
    paint.setBrush(Qt::transparent);
    qreal sz = getRadius()/4;
    paint.drawEllipse(QPointF(RectX,RectY),maxwidth,sz);
    QRect rect(RectX-maxwidth, RectY-sz,maxwidth*2,sz*2);

    paint.setFont(QFont(m_font));

    paint.drawRect(rect);
    if(getCurrentPos() == i)
    {
      paint.setPen(QColor(Colors.selectedCl));
      paint.drawText(rect, Qt::AlignCenter, strList[i]);
    }
    else
    {
      paint.setPen(QColor(Colors.textCl));
      paint.drawText(rect, Qt::AlignCenter, strList[i]);
    }
  }

  paint.setPen(QColor(Colors.selectedCl));
  paint.setBrush(QColor(Colors.switcherCl));


  // Вектор для точек треугольного переключателя
  QVector<QPoint> points;
  // Создание переключателя по 3 точкам
  points << QPoint(CenterX-getRadius()*0.7*std::cos((m_currentAngle)*M_PI/180),
                   CenterY-getRadius()*0.7*std::sin((m_currentAngle)*M_PI/180))
         << QPoint(CenterX-getRadius()*0.3*std::cos((m_currentAngle+20)*M_PI/180),
                   CenterY-getRadius()*0.3*std::sin((m_currentAngle+20)*M_PI/180))
         << QPoint(CenterX-getRadius()*0.3*std::cos((m_currentAngle-20)*M_PI/180),
                   CenterY-getRadius()*0.3*std::sin((m_currentAngle-20)*M_PI/180));


  QPolygon polygon(points);
  paint.drawPolygon(polygon);
}

void CustomTumbler::resizeEvent(QResizeEvent *event)
{
  CenterX = this->width()/2;
  CenterY = this->height()/2;
  auto value = std::min(this->width()/2, this->height()/2)*0.45;
  setRadius(value);
  m_font.setFamily("Curier");
  m_font.setPointSize(getRadius()*0.27);
  m_font.setWeight(20);
}
