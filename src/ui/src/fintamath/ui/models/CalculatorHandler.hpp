#pragma once

#include <QObject>

class CalculatorHandler : public QObject {
  Q_OBJECT

private slots:
  void onStartCalculation(const QString &str);
  void onTerminateCalculation();

signals:
  void solved(const QString &str);
  void terminateCalculation();
};
