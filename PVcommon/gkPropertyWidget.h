#ifndef GKPROPERTYWIDGET_H
#define GKPROPERTYWIDGET_H


#include "pqComponentsModule.h"
#include "pqPropertyWidget.h"

#include <QPointer>
#include <QWidget>

#include "pqPropertyLinks.h"
#include "pqDebug.h"

class pqPropertyWidgetDecorator;
class pqView;
class vtkSMDomain;
class vtkSMProperty;
class vtkSMProxy;

/// pqPropertyWidget represents a widget created for each property of a proxy on
/// the pqPropertiesPanel (for the proxy's properties or display properties).
class PQCOMPONENTS_EXPORT gkPropertyWidget : public pqPropertyWidget
{
  Q_OBJECT

public:
  gkPropertyWidget(vtkSMProxy *proxy, QWidget *parent = 0);
  virtual ~gkPropertyWidget();

  virtual void apply();
  virtual void reset();

  /// These methods are called by pqPropertiesPanel when the panel for proxy
  /// becomes active/deactive.
  /// Only widgets that have 3D widgets need to
  /// override these methods to select/deselect the 3D widgets.
  /// Default implementation does nothing.
  virtual void select() {}
  virtual void deselect() {}

  pqView* view() const;
  vtkSMProxy* proxy() const;
  vtkSMProperty* property() const;

  bool showLabel() const;

  /// Description:
  /// This static utility method returns the XML name for an object as
  /// a QString. This allows for code to get the XML name of an object
  /// without having to explicitly check for a possibly NULL char* pointer.
  ///
  /// This is templated so that it will work with a variety of objects such
  /// as vtkSMProperty's and vtkSMDomain's. It can be called with anything
  /// that has a "char* GetXMLName()" method.
  ///
  /// For example, to get the XML name of a vtkSMIntRangeDomain:
  /// QString name = pqPropertyWidget::getXMLName(domain);
  template<class T>
  static QString getXMLName(T *object)
  {
    return QString(object->GetXMLName());
  }

  /// Provides access to the decorators for this widget.
  const QList<pqPropertyWidgetDecorator*>& decorators() const
    {
    return this->Decorators;
    }

signals:
  /// This signal is emitted when the current view changes.
  void viewChanged(pqView *view);

  /// This signal is fired as soon as the user starts editing in the widget. The
  /// editing may not be complete.
  void changeAvailable();

  /// This signal is fired as soon as the user is done with making an atomic
  /// change. changeAvailable() is always fired before changeFinished().
  void changeFinished();

public slots:
  virtual void updateDependentDomains();

  /// called to set the active view. This will fire the viewChanged() signal.
  virtual void setView(pqView*);

protected:
  void addPropertyLink(QObject *qobject,
                       const char *qproperty,
                       const char *qsignal,
                       vtkSMProperty *smproperty,
                       int smindex = -1);
  void setShowLabel(bool show);

  /// For most pqPropertyWidget subclasses a changeAvailable() signal,
  /// corresponds to a changeFinished() signal. Hence by default we connect the
  /// two together. For subclasses that don't follow this pattern should call
  /// this method with 'false' to disconnect changeAvailable() and
  /// changeFinished() signals. In that case, the subclass must explicitly fire
  /// changeFinished() signal.
  void setChangeAvailableAsChangeFinished(bool status)
    { this->ChangeAvailableAsChangeFinished = status; }

private:
  /// setAutoUpdateVTKObjects no longer simply passes the flag to
  /// pqPropertyLinks. Instead we set a flag so that when this->changeFinished()
  /// is fired, we call this->apply(). Thus makes it possible for widgets with
  /// AutoUpdateVTKObjects set to true handle editing of values correctly and
  /// not push the values as the values are being edited.
  void setAutoUpdateVTKObjects(bool autoUpdate);
  void setUseUncheckedProperties(bool useUnchecked);
  void setProperty(vtkSMProperty *property);

  void addDecorator(pqPropertyWidgetDecorator*);

  friend class pqPropertiesPanel;
  friend class pqPropertyWidgetDecorator;
  friend class pqProxyWidget;

private slots:
  /// check if changeFinished() must be fired as well.
  void onChangeAvailable();

  /// if AutoUpdateVTKObjects is true, call this->apply();
  void onChangeFinished();

private:
  vtkSMProxy *Proxy;
  vtkSMProperty *Property;
  QPointer<pqView> View;
  QList<pqPropertyWidgetDecorator*> Decorators;

  pqPropertyLinks Links;
  bool ShowLabel;
  bool ChangeAvailableAsChangeFinished;
  bool AutoUpdateVTKObjects;

  /// Deprecated signals. Making private so developers get errors when they
  /// use them.
  void modified();
  void editingFinished();
};

#define PV_DEBUG_PANELS() pqDebug("PV_DEBUG_PANELS")


#endif // GKPROPERTYWIDGET_H
