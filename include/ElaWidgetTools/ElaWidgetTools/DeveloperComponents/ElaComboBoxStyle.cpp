#include "ElaComboBoxStyle.h"

#include <QDebug>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>

#include "ElaTheme.h"
ElaComboBoxStyle::ElaComboBoxStyle(QStyle* style)
{
    _pExpandIconRotate = 0;
    _pExpandMarkWidth = 0;
    _themeMode = eTheme->getThemeMode();
    connect(eTheme, &ElaTheme::themeModeChanged, this, [=](ElaThemeType::ThemeMode themeMode) {
        _themeMode = themeMode;
    });
}

ElaComboBoxStyle::~ElaComboBoxStyle()
{
}

void ElaComboBoxStyle::drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    switch (element)
    {
    case QStyle::PE_Widget:
    case QStyle::PE_PanelLineEdit:
    {
        return;
    }
#ifndef Q_OS_WIN
    case PE_PanelMenu:
    {
        return;
    }
    case PE_IndicatorArrowDown:
    {
        return;
    }
#endif
    default:
    {
        break;
    }
    }
    QProxyStyle::drawPrimitive(element, option, painter, widget);
}

void ElaComboBoxStyle::drawControl(ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    switch (element)
    {
    case QStyle::CE_ComboBoxLabel:
    {
        return;
    }
    case QStyle::CE_ShapedFrame:
    {
        //container区域
        if (widget->objectName() == "ElaComboBoxContainer")
        {
            QRect viewRect = option->rect;
            painter->save();
            painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
            eTheme->drawEffectShadow(painter, viewRect, _shadowBorderWidth, 6);
            QRect foregroundRect(viewRect.x() + _shadowBorderWidth, viewRect.y(), viewRect.width() - 2 * _shadowBorderWidth, viewRect.height() - _shadowBorderWidth);
            painter->setPen(ElaThemeColor(_themeMode, PopupBorder));
            painter->setBrush(ElaThemeColor(_themeMode, PopupBase));
            painter->drawRoundedRect(foregroundRect, 3, 3);
            painter->restore();
        }
        return;
    }
    case QStyle::CE_ItemViewItem:
    {
        //覆盖高亮
        if (const QStyleOptionViewItem* vopt = qstyleoption_cast<const QStyleOptionViewItem*>(option))
        {
            int margin = 2;
            painter->save();
            painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
            painter->setPen(Qt::NoPen);
            QPainterPath path;
            QRect optionRect = option->rect;
            optionRect.adjust(margin, margin, -margin, -margin);
#ifndef Q_OS_WIN
            optionRect.adjust(6, 0, -6, 0);
#endif
            path.addRoundedRect(optionRect, 5, 5);
            if (option->state & QStyle::State_Selected)
            {
                if (option->state & QStyle::State_MouseOver)
                {
                    // 选中时覆�?
                    painter->setBrush(ElaThemeColor(_themeMode, BasicSelectedHoverAlpha));
                    painter->drawPath(path);
                }
                else
                {
                    // 选中
                    painter->setBrush(ElaThemeColor(_themeMode, BasicSelectedAlpha));
                    painter->drawPath(path);
                }
                //选中Mark
                painter->setPen(Qt::NoPen);
                painter->setBrush(ElaThemeColor(_themeMode, PrimaryNormal));
                painter->drawRoundedRect(QRectF(optionRect.x() + 3, optionRect.y() + optionRect.height() * 0.2, 3, optionRect.height() - +optionRect.height() * 0.4), 2, 2);
            }
            else
            {
                if (option->state & QStyle::State_MouseOver)
                {
                    // 覆盖时颜�?
                    painter->setBrush(ElaThemeColor(_themeMode, BasicHoverAlpha));
                    painter->drawPath(path);
                }
            }
            // 文字绘制
            painter->setPen(ElaThemeColor(_themeMode, BasicText));
            painter->drawText(QRect(option->rect.x() + 15, option->rect.y(), option->rect.width() - 15, option->rect.height()), Qt::AlignVCenter, vopt->text);
            painter->restore();
        }
        return;
    }
    default:
    {
        break;
    }
    }
    QProxyStyle::drawControl(element, option, painter, widget);
}

void ElaComboBoxStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget) const
{
    switch (control)
    {
    case QStyle::CC_ComboBox:
    {
        //主体显示绘制
        if (const QStyleOptionComboBox* copt = qstyleoption_cast<const QStyleOptionComboBox*>(option))
        {
            painter->save();
            painter->setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing | QPainter::TextAntialiasing);
            //背景绘制
            bool isEnabled = copt->state.testFlag(QStyle::State_Enabled);
            painter->setPen(ElaThemeColor(_themeMode, BasicBorder));
            painter->setBrush(isEnabled ? (copt->state.testFlag(QStyle::State_HasFocus) && copt->editable)
                                      ? ElaThemeColor(_themeMode, DialogBase)
                                      : copt->state.testFlag(QStyle::State_MouseOver)
                                      ? ElaThemeColor(_themeMode, BasicHover)
                                      : ElaThemeColor(_themeMode, BasicBase)
                                        : ElaThemeColor(_themeMode, BasicDisable));
            QRect comboBoxRect = copt->rect;
            comboBoxRect.adjust(_shadowBorderWidth, 1, -_shadowBorderWidth, -1);
            painter->drawRoundedRect(comboBoxRect, 3, 3);
            // 底边线绘�?
            painter->setPen(ElaThemeColor(_themeMode, BasicBaseLine));
            painter->drawLine(comboBoxRect.x() + 3, comboBoxRect.y() + comboBoxRect.height(), comboBoxRect.x() + comboBoxRect.width() - 3, comboBoxRect.y() + comboBoxRect.height());

            //文字绘制
            if (!copt->editable)
            {
                QRect textRect = subControlRect(QStyle::CC_ComboBox, copt, QStyle::SC_ScrollBarSubLine, widget);
                painter->setPen(isEnabled ? ElaThemeColor(_themeMode, BasicText) : ElaThemeColor(_themeMode, BasicTextDisable));
                painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, copt->currentText);
            }
            //展开指示器绘�?
            painter->setPen(Qt::NoPen);
            painter->setBrush(ElaThemeColor(_themeMode, PrimaryNormal));
            painter->drawRoundedRect(QRectF(comboBoxRect.center().x() - _pExpandMarkWidth, comboBoxRect.y() + comboBoxRect.height() - 3, _pExpandMarkWidth * 2, 3), 2, 2);
            // 展开图标绘制
            QRect expandIconRect = subControlRect(QStyle::CC_ComboBox, copt, QStyle::SC_ScrollBarAddPage, widget);
            if (expandIconRect.isValid())
            {
                QFont iconFont = QFont("ElaAwesome");
                iconFont.setPixelSize(17);
                painter->setFont(iconFont);
                painter->setPen(isEnabled ? ElaThemeColor(_themeMode, BasicText) : ElaThemeColor(_themeMode, BasicTextDisable));
                painter->translate(expandIconRect.x() + (qreal)expandIconRect.width() / 2, expandIconRect.y() + (qreal)expandIconRect.height() / 2);
                painter->rotate(_pExpandIconRotate);
                painter->translate(-expandIconRect.x() - (qreal)expandIconRect.width() / 2, -expandIconRect.y() - (qreal)expandIconRect.height() / 2);
                painter->drawText(expandIconRect, Qt::AlignCenter, QChar(static_cast<char16_t>(ElaIconType::AngleDown)));
                painter->restore();
            }
        }
        return;
    }
    default:
    {
        break;
    }
    }
    QProxyStyle::drawComplexControl(control, option, painter, widget);
}

QRect ElaComboBoxStyle::subControlRect(ComplexControl cc, const QStyleOptionComplex* opt, SubControl sc, const QWidget* widget) const
{
    switch (cc)
    {
    case QStyle::CC_ComboBox:
    {
        switch (sc)
        {
        case QStyle::SC_ScrollBarSubLine:
        {
            //文字区域
            QRect textRect = QProxyStyle::subControlRect(cc, opt, sc, widget);
            textRect.setLeft(16);
            textRect.setRight(textRect.right() - 15);
            return textRect;
        }
        case QStyle::SC_ScrollBarAddPage:
        {
            //展开图标区域
            QRect expandIconRect = QProxyStyle::subControlRect(cc, opt, sc, widget);
            expandIconRect.setLeft(expandIconRect.left() - 25);
            return expandIconRect;
        }
        default:
        {
            break;
        }
        }
        break;
    }
    default:
    {
        break;
    }
    }

    return QProxyStyle::subControlRect(cc, opt, sc, widget);
}

QSize ElaComboBoxStyle::sizeFromContents(ContentsType type, const QStyleOption* option, const QSize& size, const QWidget* widget) const
{
    switch (type)
    {
    case QStyle::CT_ItemViewItem:
    {
        QSize itemSize = QProxyStyle::sizeFromContents(type, option, size, widget);
        itemSize.setHeight(35);
        return itemSize;
    }
    case QStyle::CT_ComboBox:
    {
        QSize comboBoxSize = QProxyStyle::sizeFromContents(type, option, size, widget);
        comboBoxSize.setWidth(comboBoxSize.width() + 26);
        return comboBoxSize;
    }
    default:
    {
        break;
    }
    }
    return QProxyStyle::sizeFromContents(type, option, size, widget);
}
