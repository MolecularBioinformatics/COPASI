// Copyright (C) 2017 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and University of
// of Connecticut School of Medicine.
// All rights reserved.

// Copyright (C) 2013 - 2016 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

#include <QGraphicsItem>
#include <QGraphicsEffect>

#include <qlayout/CQEffectDescription.h>
#include <qlayout/CQCopasiEffect.h>
#include <qlayout/CQConnectionGraphicsItem.h>

qreal linear(qreal a, qreal b, qreal t)
{
  return a * (1 - t) + b * t;
}

/**
 * Interpolate between a, and b
 *
 * 0.0 <= t <= 1.0
 */
QColor interpolate(const QColor &a, const QColor &b, float t)
{
  int ah, as, av, aa;
  a.getHsv(&ah, &as, &av, &aa);

  int bh, bs, bv, ba;
  b.getHsv(&bh, &bs, &bv, &ba);

  int rh, rs, rv, ra;
  rh = linear(ah, bh, t);
  rs = linear(as, bs, t);
  rv = linear(av, bv, t);
  ra = linear(aa, ba, t);

  return QColor::fromHsv(rh, rs, rv, ra);
}

CQEffectDescription::CQEffectDescription(const std::string& cn, Mode mode, const QColor &startColor, const QColor& endColor)
  : mCN(cn)
  , mStartColor(startColor)
  , mEndColor(endColor)
  , mScaleStart(0.5)
  , mScaleEnd(2.0)
  , mMode(mode)
{
}

CQEffectDescription::CQEffectDescription(const std::string& cn, qreal startScale, qreal endScale)
  : mCN(cn)
  , mStartColor(Qt::white)
  , mEndColor(Qt::red)
  , mScaleStart(startScale)
  , mScaleEnd(endScale)
  , mMode(Scale)
{
}

CQEffectDescription::~CQEffectDescription()
{
}

const std::string& CQEffectDescription::getCN()const
{
  return mCN;
}

const QColor& CQEffectDescription::getStartColor() const
{
  return mStartColor;
}

const QColor& CQEffectDescription::getEndColor() const
{
  return mEndColor;
}

qreal CQEffectDescription::getScaleStart() const
{
  return mScaleStart;
}

qreal CQEffectDescription::getScaleEnd() const
{
  return mScaleEnd;
}

CQEffectDescription::Mode CQEffectDescription::getMode() const
{
  return mMode;
}
void CQEffectDescription::setMode(CQEffectDescription::Mode mode)
{
  mMode = mode;
}

void CQEffectDescription::removeFromScene(CQLayoutScene& scene)
{
  QGraphicsItem *item = scene.getItemFor(mCN);

  if (item == NULL)
    return;

  QGraphicsEffect* effect = item->graphicsEffect();

  if (effect == NULL)
    return;

  effect->setEnabled(false);
}

void CQEffectDescription::applyToScene(CQLayoutScene& scene, qreal t)
{
  QGraphicsItem *item = scene.getItemFor(mCN);

  if (item == NULL)
    return;

  CQConnectionGraphicsItem* connItem = dynamic_cast<CQConnectionGraphicsItem*>(item);

  if (connItem)
    connItem->setUseFullShape(true);

  switch (mMode)
    {
      default:
      case DropShadow:
      {
        QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect(&scene);
        effect->setColor(interpolate(mStartColor, mEndColor, t));
        effect->setBlurRadius(25);
        effect->setEnabled(true);
        effect->setOffset(0);
        item->setGraphicsEffect(effect);
      }
      break;

      case Colorize:
      {
        QGraphicsColorizeEffect* effect = new QGraphicsColorizeEffect(&scene);
        effect->setColor(interpolate(mStartColor, mEndColor, t));
        effect->setStrength(1);
        effect->setEnabled(true);
        item->setGraphicsEffect(effect);
      }
      break;

      case Scale:
      {
        CQCopasiEffect* effect = new CQCopasiEffect();
        effect->setScale(linear(mScaleStart, mScaleEnd, t));
        effect->setEnabled(true);
        item->setGraphicsEffect(effect);
      }
      break;
    }
}

void CQEffectDescription::setCN(const std::string& cn)
{
  mCN = cn;
}

void CQEffectDescription::setStartColor(const QColor& color)
{
  mStartColor = color;
}

void CQEffectDescription::setEndColor(const QColor& color)
{
  mEndColor = color;
}

void CQEffectDescription::setScaleStart(qreal scale)
{
  mScaleStart = scale;
}

void CQEffectDescription::setScaleEnd(qreal scale)
{
  mScaleEnd = scale;
}
