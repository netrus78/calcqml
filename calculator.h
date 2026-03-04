#pragma once
#include <QObject>
#include <QString>
#include <stack>
#include <QTimer>
#include <boost/multiprecision/cpp_dec_float.hpp>

using boost::multiprecision::cpp_dec_float_50;

class Calculator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString display READ display NOTIFY displayChanged)
    Q_PROPERTY(QString expression READ expression NOTIFY expressionChanged)

public:
    explicit Calculator(QObject *parent = nullptr);

    QString display() const;
    QString expression() const;

    Q_INVOKABLE void inputNumber(QString num);
    Q_INVOKABLE void inputOperation(QString op);
    Q_INVOKABLE void inputDot();
    Q_INVOKABLE void clear();
    Q_INVOKABLE void calculate();
    Q_INVOKABLE void plusMinus();
    Q_INVOKABLE void percent();

    Q_INVOKABLE void equalsPressed();
    Q_INVOKABLE void equalsPressedAndHeld();
    Q_INVOKABLE void equalsReleased();

signals:
    void displayChanged();
    void expressionChanged();
    void showSecretMenu();

private:
    QString m_display = "0";
    QString m_expression;

    int precedence(char op);
    cpp_dec_float_50 evaluateExpression(const std::string &expr);

    QTimer m_longPressTimer;
    QTimer m_secretInputTimer;
    bool m_longPressDetected = false;
    QString m_secretBuffer;
};
