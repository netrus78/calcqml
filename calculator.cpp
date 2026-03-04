#include "calculator.h"
#include <cctype>

Calculator::Calculator(QObject *parent) : QObject(parent)
{
    m_longPressTimer.setSingleShot(true);
    m_longPressTimer.setInterval(4000);

    connect(&m_longPressTimer, &QTimer::timeout, [this]() {
        m_longPressDetected = true;
        m_secretBuffer.clear();
        m_secretInputTimer.start();
    });

    m_secretInputTimer.setSingleShot(true);
    m_secretInputTimer.setInterval(5000);
    connect(&m_secretInputTimer, &QTimer::timeout, [this]() {
        m_longPressDetected = false;
        m_secretBuffer.clear();
    });
}

QString Calculator::display() const { return m_display; }
QString Calculator::expression() const { return m_expression; }

void Calculator::inputNumber(QString num)
{
    if (m_display == "0")
        m_display = num;
    else
        m_display += num;

    m_expression += num;

    if (m_longPressDetected) {
        m_secretBuffer += num;

        if (!QString("123").startsWith(m_secretBuffer))
            m_secretBuffer.clear();

        if (m_secretBuffer == "123") {
            emit showSecretMenu();
            m_longPressDetected = false;
            m_secretBuffer.clear();
            m_secretInputTimer.stop();
        }
    }

    emit displayChanged();
    emit expressionChanged();
}

void Calculator::inputOperation(QString op)
{
    if (m_expression.isEmpty()) return;
    if (op == "x") op = "*";
    if (op == "÷") op = "/";

    if (QString("+-*/").contains(m_expression.back()))
        return;

    m_expression += op;
    m_display = "0";

    emit displayChanged();
    emit expressionChanged();
}

void Calculator::inputDot()
{
    if (m_display.contains(".")) return;

    m_display += ".";
    m_expression += ".";

    emit displayChanged();
    emit expressionChanged();
}

void Calculator::clear()
{
    m_display = "0";
    m_expression.clear();

    emit displayChanged();
    emit expressionChanged();
}

void Calculator::calculate()
{
    if (m_expression.isEmpty()) return;

    cpp_dec_float_50 result = evaluateExpression(m_expression.toStdString());
    m_display = QString::fromStdString(result.str());

    emit displayChanged();
}

void Calculator::plusMinus()
{
    if (m_display == "0") return;

    if (m_display.startsWith("-"))
        m_display.remove(0, 1);
    else
        m_display = "-" + m_display;

    emit displayChanged();
}

void Calculator::percent()
{
    cpp_dec_float_50 val(m_display.toStdString());
    val /= 100;
    m_display = QString::fromStdString(val.str());
    emit displayChanged();
}

int Calculator::precedence(char op)
{
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

cpp_dec_float_50 Calculator::evaluateExpression(const std::string &expr)
{
    std::stack<cpp_dec_float_50> values;
    std::stack<char> ops;

    auto applyOp = [&]() {
        auto b = values.top(); values.pop();
        auto a = values.top(); values.pop();
        char op = ops.top(); ops.pop();

        if (op == '+') values.push(a + b);
        if (op == '-') values.push(a - b);
        if (op == '*') values.push(a * b);
        if (op == '/') values.push(a / b);
    };

    for (size_t i = 0; i < expr.size();) {
        if (isspace(expr[i])) { i++; continue; }

        if (isdigit(expr[i]) || expr[i] == '.') {
            std::string num;
            while (i < expr.size() && (isdigit(expr[i]) || expr[i] == '.'))
                num += expr[i++];
            values.push(cpp_dec_float_50(num));
            continue;
        }

        if (expr[i] == '(') { ops.push('('); i++; continue; }

        if (expr[i] == ')') {
            while (!ops.empty() && ops.top() != '(')
                applyOp();
            ops.pop();
            i++;
            continue;
        }

        while (!ops.empty() && precedence(ops.top()) >= precedence(expr[i]))
            applyOp();

        ops.push(expr[i++]);
    }

    while (!ops.empty()) applyOp();
    return values.top();
}

void Calculator::equalsPressedAndHeld()
{
    m_longPressDetected = false;
    m_longPressTimer.start();
}

void Calculator::equalsReleased()
{
    if (!m_longPressDetected)
        calculate();

    m_longPressTimer.stop();
}

void Calculator::equalsPressed()
{
    if (!m_longPressDetected)
        calculate();
}
