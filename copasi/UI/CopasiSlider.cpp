/* Begin CVS Header
   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/UI/CopasiSlider.cpp,v $
   $Revision: 1.5 $
   $Name:  $
   $Author: gauges $ 
   $Date: 2004/11/25 14:33:42 $
   End CVS Header */

#include <cmath>

#include "qlabel.h"
#include "qstring.h"
#include "qslider.h"

#include "CopasiSlider.h"
#include "qtUtilities.h"
#include "report/CCopasiObject.h"
#include "report/CCopasiObjectReference.h"
#include "utilities/CCopasiParameterGroup.h"
#include "report/CCopasiObjectName.h"

CopasiSlider::CopasiSlider(CCopasiObject* object, QWidget* parent): QVBox(parent), mpObject(object) , mpSlider(NULL), mpLabel(NULL), mpParameterGroup(NULL)
{
  this->mpLabel = new QLabel(this);
  this->mpLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
  this->mpSlider = new QSlider(Qt::Horizontal, this);
  this->mpSlider->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
  this->updateSliderData();

  connect(this->mpSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged(int)));
  connect(this->mpSlider, SIGNAL(sliderReleased()), this, SLOT(qSliderReleased()));
}

CopasiSlider::~CopasiSlider()
{
  delete this->mpSlider;
  delete this->mpLabel;
}

void CopasiSlider::updateSliderData()
{
  if (this->mpObject)
    {
      if (!this->mpParameterGroup)
        {
          this->mpParameterGroup = new CCopasiParameterGroup("slider");
          this->mpParameterGroup->addParameter("object", CCopasiParameter::STRING, this->mpObject->getCN());
          this->mpParameterGroup->addParameter("type", CCopasiParameter::STRING, CCopasiParameter::XMLType[CCopasiParameter::INVALID]);
          this->mValue = 0.0;
          if (this->mpObject->isValueDbl())
            {
              this->mpParameterGroup->setValue("type", CCopasiParameter::XMLType[CCopasiParameter::DOUBLE]);
              this->mValue = *(double*)this->mpObject->getReference();
            }
          else if (this->mpObject->isValueInt())
            {
              this->mpParameterGroup->setValue("type", CCopasiParameter::XMLType[CCopasiParameter::INT]);
              this->mValue = (double)(*(int*)this->mpObject->getReference());
            }
          this->mMinValue = 0.0;
          this->mpParameterGroup->addParameter("value", CCopasiParameter::DOUBLE, this->mValue);
          this->mpParameterGroup->addParameter("minValue", CCopasiParameter::DOUBLE, this->mMinValue);
          this->mMaxValue = 2 * this->mValue;
          this->mpParameterGroup->addParameter("maxValue", CCopasiParameter::DOUBLE, 2.0*this->mMaxValue);
          this->mNumMinorTicks = 100;
          this->mMinorMajorFactor = 10;
          this->mpParameterGroup->addParameter("numMinorTicks", CCopasiParameter::UINT, (C_INT32)this->mNumMinorTicks);
          this->mpParameterGroup->addParameter("minorMajorFactor", CCopasiParameter::UINT, this->mMinorMajorFactor);
        }
      this->mpSlider->setMinValue(0);
      this->mpSlider->setMaxValue(this->mNumMinorTicks);
      this->mpSlider->setTickInterval(1);
      this->mpSlider->setLineStep(1);
      this->mpSlider->setPageStep(this->mMinorMajorFactor);
      this->mTickInterval = (this->mMaxValue - this->mMinValue) / this->mNumMinorTicks;
      this->mpSlider->setValue((int)floor(((this->mValue - this->mMinValue) / this->mTickInterval) + 0.5));
      if (this->mpObject->isValueInt())
        {
          this->setType(CCopasiParameter::INT);
        }
      else if (this->mpObject->isValueDbl())
        {
          this->setType(CCopasiParameter::DOUBLE);
        }
      else
        {
          this->setEnabled(false);
        }
    }
  this->updateLabel();
}

C_FLOAT64 CopasiSlider::value() const
  {
    return this->mValue;
  }

void CopasiSlider::setValue(C_FLOAT64 value)
{
  if (value < this->mMinValue)
    {
      value = this->mMinValue;
    }
  else if (value > this->mMaxValue)
    {
      value = this->mMaxValue;
    }
  this->mValue = value;
  this->mpParameterGroup->setValue("value", this->mValue);
  this->mpSlider->setValue((int)floor(((this->mValue - this->mMinValue) / this->mTickInterval) + 0.5));
  if (this->mType == CCopasiParameter::INT)
    {
      C_INT32* reference = (C_INT32*)(((CCopasiObjectReference<C_INT32>*)this->mpObject)->getReference());

      *reference = (C_INT32)floor(this->mValue + 0.5);
    }
  else if (this->mType == CCopasiParameter::DOUBLE)
    {
      C_FLOAT64* reference = (C_FLOAT64*)(((CCopasiObjectReference<C_FLOAT64>*)this->mpObject)->getReference());

      *reference = this->mValue;
    }
  this->updateLabel();
}

unsigned C_INT32 CopasiSlider::minorMajorFactor() const
  {
    return this->mMinorMajorFactor;
  }

void CopasiSlider::setMinorMajorFactor(unsigned C_INT32 factor)
{
  this->mMinorMajorFactor = factor;
  this->mpParameterGroup->setValue("minorMajorFactor", this->mMinorMajorFactor);
  this->mpSlider->setPageStep(this->mpSlider->lineStep()*this->mMinorMajorFactor);
}

C_FLOAT64 CopasiSlider::minorTickInterval() const
  {
    return (C_FLOAT64)(this->mMaxValue - this->mMinValue) / ((C_FLOAT64)this->mNumMinorTicks);
  }

void CopasiSlider::setNumMinorTicks(unsigned C_INT32 numMinorTicks)
{
  this->mNumMinorTicks = numMinorTicks;
  this->mpParameterGroup->setValue("numMinorTicks", this->mNumMinorTicks);
  // set maxValue and value of slider
  this->mpSlider->setMaxValue(numMinorTicks);
  this->mpSlider->setValue((int)floor(((this->mValue - this->mMinValue) / this->mTickInterval) + 0.5));
}

unsigned C_INT32 CopasiSlider::numMinorTicks() const
  {
    return this->mNumMinorTicks;
  }

C_FLOAT64 CopasiSlider::minValue() const
  {
    return this->mMinValue;
  }

C_FLOAT64 CopasiSlider::maxValue() const
  {
    return this->mMaxValue;
  }

CCopasiObject* CopasiSlider::object() const
  {
    return this->mpObject;
  }

void CopasiSlider::setObject(CCopasiObject* object)
{
  this->mpObject = object;
  this->mpParameterGroup->setValue("object", this->mpObject->getCN());
  this->updateSliderData();
}

void CopasiSlider::setMaxValue(C_FLOAT64 value)
{
  if (value <= mMinValue) return;

  this->mMaxValue = value;

  this->mpParameterGroup->setValue("maxValue", this->mMaxValue);

  this->mTickInterval = (this->mMaxValue - this->mMinValue) / this->mNumMinorTicks;

  if (this->value() > this->mMaxValue)
    {
      this->setValue(this->mMaxValue);
    }

  this->mpSlider->setValue((int)floor(((this->mValue - this->mMinValue) / this->mTickInterval) + 0.5));

  this->updateLabel();
}

void CopasiSlider::setMinValue(C_FLOAT64 value)
{
  if (value >= mMaxValue) return;

  this->mMinValue = value;

  this->mpParameterGroup->setValue("minValue", this->mMinValue);

  this->mTickInterval = (this->mMaxValue - this->mMinValue) / this->mNumMinorTicks;

  if (this->value() < this->mMinValue)
    {
      this->setValue(this->mMinValue);
    }

  this->mpSlider->setValue((int)floor(((this->mValue - this->mMinValue) / this->mTickInterval) + 0.5));

  this->updateLabel();
}

void CopasiSlider::updateLabel()
{
  QString labelString = "";
  if (this->mpObject)
    {
      labelString += FROM_UTF8(this->mpObject->getObjectName());
    }
  labelString += " : [";
  labelString += QString::number(this->mMinValue);
  labelString += "-";
  labelString += QString::number(this->mMaxValue);
  labelString += "] {";
  labelString += QString::number(this->value());
  labelString += "}";
  this->mpLabel->setText(labelString);
}

void CopasiSlider::sliderValueChanged(int value)
{
  this->mValue = this->mMinValue + value * this->mTickInterval;

  this->mpParameterGroup->setValue("value", this->mValue);

  if (this->mType == CCopasiParameter::INT)
    {
      C_INT32* reference = (C_INT32*)(((CCopasiObjectReference<C_INT32>*)this->mpObject)->getReference());

      *reference = (C_INT32)floor(this->mValue + 0.5);
    }
  else if (this->mType == CCopasiParameter::DOUBLE)
    {
      C_FLOAT64* reference = (C_FLOAT64*)(((CCopasiObjectReference<C_FLOAT64>*)this->mpObject)->getReference());

      *reference = this->mValue;
    }
  this->updateLabel();

  emit valueChanged(this->mValue);
}

void CopasiSlider::qSliderReleased()
{
  emit sliderReleased();
}

CCopasiParameter::Type CopasiSlider::type() const
  {
    return this->mType;
  }

void CopasiSlider::setType(CCopasiParameter::Type type)
{
  this->mType = type;
  this->mpParameterGroup->setValue("type", CCopasiParameter::XMLType[this->mType]);
}
