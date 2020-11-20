#ifndef CUSTOMTUMBLER_H
#define CUSTOMTUMBLER_H

#include <QWidget>
#include <QPainter>
#include "stylescolors.h"

//Компонент тумблер для возможности переключения позиций и выдачи текущего выставленного индекса
//(Используется как для значений так и для строк текста)
class CustomTumbler : public QWidget
{
  Q_OBJECT
public:
  //Конструктор
  explicit CustomTumbler(QWidget *parent = nullptr);

    //Перечисление типов стилей
    enum TYPE_STYLE
    {
      Dracula = 1,
      SnowWhite   = 2
    };

  //Getters
  qreal getStartAngle() const
  {
    return m_startAngle;
  }
  qreal getEndAngle() const
  {
    return m_endAngle;
  }
  unsigned getPosAmount() const
  {
    return m_PosAmount;
  }
  QStringList getLabels() const
  {
    return m_labels;
  }
  int getCurrentPos() const
  {
    return m_currentPos;
  }

  //Setters
  void  setStartAngle(qreal &angle)
  {
    m_startAngle = angle;
  }
  void  setEndAngle(qreal &angle)
  {
    m_endAngle = angle;
  }
  void  setPosAmount(int &amount)
  {
    if(amount >= 1)
       m_PosAmount = amount;
    else
      m_PosAmount = 1;
  }
  void  setLabels(QStringList &lbls)
  {
    m_labels = lbls;
  }
  void  setCurrentPos(int index){
    if(index >= 0 && index < m_vect.size())
      m_currentPos = index;
    else
      m_currentPos = 0;
  }


  //------------------------------------PUBLIC_FUNCTIONS-------------------------------------------------
  void setDefaultValues(qreal startAngle, qreal endAngle, QStringList& labels, int currentPosition);
  void setStyle(unsigned int type);
  //-----------------------------------------------------------------------------------------------------

private:
  // пара, используется только для объявления других пар
  typedef std::pair<QString, qreal> pairType;

  /// вектор для хранения пар [индекс]->
  /// [("Название позиции", "Угол, на котором находится позиция")];
  QVector<pairType> m_vect;

  // Координаты центра компонента
  qreal CenterX;
  qreal CenterY;

  //Радиус круга
  qreal m_Radius;
  //Текущий угол
  qreal m_currentAngle;
  //Текущая позиция
  int   m_currentPos;
  //Количество значений
  unsigned    m_PosAmount;
  //Начальный угол
  qreal       m_startAngle;
  //Конечный угол
  qreal       m_endAngle;
  //Cписок строк
  QStringList m_labels;
  //Текущий фонт
  QFont       m_font;
  //Флаг нажатия
  bool        m_pressed;

private:

  qreal getRadius() const
  {
    return m_Radius;
  }
  void  setRadius(qreal &R)
  {
    m_Radius = R;
  }
  void  setPressed(bool flag){m_pressed = flag;}

  //----------------------PRIVATE_FUNCTIONS--------------------
  bool isPressed(){return m_pressed;}
  void correctNegativeAngles();
  void calcPos(QMouseEvent *event);
  //-----------------------------------------------------------

  //Структура цветов
  struct{
    QColor textCl;
    QColor selectedCl;
    QColor posCl;
    QColor posBorderCl;
    QColor borderCl;
    QColor centralObjCl;
    QColor switcherCl;
    QColor backGroundCl;
  }Colors;

protected:

  //Events
  void mousePressEvent(QMouseEvent *event)       override; //Срабатывание на нажатие мыши на компоненте
  void mouseReleaseEvent(QMouseEvent *event)     override; //Срабатывание на отжатие кнопки мыши
  void mouseMoveEvent (QMouseEvent *event)       override; //Срабатывание на движение мыши по компоненту
  void keyPressEvent  (QKeyEvent *_event)        override; //Срабатывание на нажатия кнопки (стрелки, пад)
  void wheelEvent     (QWheelEvent *_event)      override; //Срабатывание на прокрутку колеса мыши
  void paintEvent     (QPaintEvent *event)       override; //Срабатывание на отрисовку формы
  void resizeEvent    (QResizeEvent *event)      override; //Срабатывание на изменения размера формы компонента


signals:

public slots:
};

#endif // CUSTOMTUMBLER_H
