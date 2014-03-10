/*=========================================================================

   Program: ParaView
   Module: pqPropertyWidget.cxx

   Copyright (c) 2005-2012 Sandia Corporation, Kitware Inc.
   All rights reserved.

   ParaView is a free software; you can redistribute it and/or modify it
   under the terms of the ParaView license version 1.2.

   See License_v1.2.txt for the full ParaView license.
   A copy of this license can be obtained by contacting
   Kitware Inc.
   28 Corporate Drive
   Clifton Park, NY 12065
   USA

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


*** MODIFIED BY JOSHUA MURPHY FOR USE IN GHOST KIT, 10 MAR 2014 ***
=========================================================================*/
#include "gkPropertyWidget.h"

#include "pqPropertiesPanel.h"
#include "pqPropertyWidgetDecorator.h"
#include "pqProxy.h"
#include "pqUndoStack.h"
#include "pqView.h"
#include "vtkSMDocumentation.h"
#include "vtkSMDomain.h"
#include "vtkSMProperty.h"

//-----------------------------------------------------------------------------
gkPropertyWidget::gkPropertyWidget(vtkSMProxy *smProxy, QWidget *parentObject)
    : pqPropertyWidget(smProxy, parentObject),
      Proxy(smProxy),
      Property(0),
      ChangeAvailableAsChangeFinished(true),
      AutoUpdateVTKObjects(true)
{
    this->ShowLabel = true;
    this->Links.setAutoUpdateVTKObjects(false);
    this->Links.setUseUncheckedProperties(true);

    this->connect(&this->Links, SIGNAL(qtWidgetChanged()),
                  this, SIGNAL(changeAvailable()));

    this->connect(&this->Links, SIGNAL(qtWidgetChanged()),
                  this, SLOT(onChangeAvailable()));

    this->connect(this, SIGNAL(changeFinished()),
                  this, SLOT(onChangeFinished()));

    //force this value
    this->setUseUncheckedProperties(false);
    this->setAutoUpdateVTKObjects(true);
}

//-----------------------------------------------------------------------------
gkPropertyWidget::~gkPropertyWidget()
{
    foreach (pqPropertyWidgetDecorator* decorator, this->Decorators)
    {
        delete decorator;
    }

    this->Decorators.clear();
}

//-----------------------------------------------------------------------------
void gkPropertyWidget::onChangeAvailable()
{
    if (this->ChangeAvailableAsChangeFinished)
    {
        emit this->changeFinished();
    }
}

//-----------------------------------------------------------------------------
void gkPropertyWidget::onChangeFinished()
{
    if (this->AutoUpdateVTKObjects)
    {
        BEGIN_UNDO_SET("Property Changed");
        this->apply();
        END_UNDO_SET();
    }
}

//-----------------------------------------------------------------------------
pqView* gkPropertyWidget::view() const
{
    return this->View;
}

//-----------------------------------------------------------------------------
void gkPropertyWidget::setView(pqView* pqview)
{
    this->View = pqview;
    emit this->viewChanged(pqview);
}

//-----------------------------------------------------------------------------
vtkSMProxy* gkPropertyWidget::proxy() const
{
    return this->Proxy;
}

//-----------------------------------------------------------------------------
void gkPropertyWidget::setProperty(vtkSMProperty *smproperty)
{
    this->Property = smproperty;
    if (smproperty && smproperty->GetDocumentation())
    {
        QString doc = smproperty->GetDocumentation()->GetDescription();
        doc = doc.trimmed();
        doc = doc.replace(QRegExp("\\s+")," ");
        this->setToolTip(
                    QString("<html><head/><body><p align=\"justify\">%1</p></body></html>").arg(doc));
    }
}

//-----------------------------------------------------------------------------
vtkSMProperty* gkPropertyWidget::property() const
{
    return this->Property;
}

//-----------------------------------------------------------------------------
void gkPropertyWidget::apply()
{
    this->Links.accept();
}

//-----------------------------------------------------------------------------
void gkPropertyWidget::reset()
{
    this->Links.reset();
}

//-----------------------------------------------------------------------------
void gkPropertyWidget::setShowLabel(bool isLabelVisible)
{
    this->ShowLabel = isLabelVisible;
}

//-----------------------------------------------------------------------------
bool gkPropertyWidget::showLabel() const
{
    return this->ShowLabel;
}

//-----------------------------------------------------------------------------
void gkPropertyWidget::addPropertyLink(QObject *qobject,
                                       const char *qproperty,
                                       const char *qsignal,
                                       vtkSMProperty *smproperty,
                                       int smindex)
{
    this->Links.addPropertyLink(qobject,
                                qproperty,
                                qsignal,
                                this->Proxy,
                                smproperty,
                                smindex);
}

//-----------------------------------------------------------------------------
void gkPropertyWidget::setAutoUpdateVTKObjects(bool autoUpdate)
{
//      enable this behaviour by default -- JJM 10 MAR 2014
      this->AutoUpdateVTKObjects = true;
}

//-----------------------------------------------------------------------------
void gkPropertyWidget::setUseUncheckedProperties(bool useUnchecked)
{
    //  disable this behaviour by default -- JJM 10 MAR 2014
      this->Links.setUseUncheckedProperties(false);
}

//-----------------------------------------------------------------------------
void gkPropertyWidget::updateDependentDomains()
{
    if(this->Property)
    {
        this->Property->UpdateDependentDomains();
    }
}

//-----------------------------------------------------------------------------
void gkPropertyWidget::addDecorator(pqPropertyWidgetDecorator* decorator)
{
    if (!decorator || decorator->parent() != this)
    {
        qCritical("Either the decorator is NULL or has an invalid parent."
                  "Please check the code.");
    }
    else
    {
        this->Decorators.push_back(decorator);
    }
}
