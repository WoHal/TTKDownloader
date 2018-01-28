#include "downloadwidgetutils.h"

#include <QComboBox>
#include <QBitmap>
#include <QPainter>
#include <QBuffer>
#include <QFileDialog>

void DownloadUtils::Widget::setLabelFontSize(QWidget *widget, int size)
{
    QFont font = widget->font();
    font.setPointSize(size);
    widget->setFont(font);
}

QString DownloadUtils::Widget::elidedText(const QFont &font, const QString &text,
                                          Qt::TextElideMode mode, int width)
{
    QFontMetrics ft(font);
    return ft.elidedText(text, mode, width);
}

void DownloadUtils::Widget::setTransparent(QWidget *widget, int alpha)
{
    QPalette pal = widget->palette();
    pal.setBrush(QPalette::Base, QBrush(QColor(0xFF, 0xFF, 0xFF, alpha)));
    widget->setPalette(pal);
}

void DownloadUtils::Widget::setComboboxText(QComboBox *object, const QString &text)
{
    if(object->isEditable())
    {
        object->setEditText(text);
    }
    else
    {
        const int i = object->findText(text);
        if(i > -1)
        {
            object->setCurrentIndex(i);
        }
    }
}

void DownloadUtils::Widget::widgetToRound(QWidget *w, int ratioX, int ratioY)
{
    w->setMask( getBitmapMask(w->rect(), ratioX, ratioY) );
}

void DownloadUtils::Widget::fusionPixmap(QPixmap &bg, const QPixmap &fg, const QPoint &pt)
{
    QPainter painter(&bg);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter.drawPixmap(pt.x(), pt.y(), fg);
    painter.end();
}

QPixmap DownloadUtils::Widget::pixmapToRound(const QPixmap &src, const QSize &size, int ratioX, int ratioY)
{
    return pixmapToRound(src, QRect(QPoint(0, 0), size), ratioX, ratioY);
}

QPixmap DownloadUtils::Widget::pixmapToRound(const QPixmap &src, const QRect &rect, int ratioX, int ratioY)
{
    if(src.isNull())
    {
        return QPixmap();
    }

    QPixmap image = src.scaled(rect.size());
    image.setMask( getBitmapMask(rect, ratioX, ratioY) );
    return image;
}

QPixmap DownloadUtils::Widget::pixmapToRound(const QPixmap &src, const QPixmap &mask, const QSize &size)
{
    if(src.isNull())
    {
        return QPixmap();
    }

    QPixmap image(mask);
    QPainter painter(&image);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter.drawPixmap(0, 0, src.scaled(size));
    painter.end();

    return image;
}

QBitmap DownloadUtils::Widget::getBitmapMask(const QRect &rect, int ratioX, int ratioY)
{
    QBitmap mask(rect.size());
    QPainter painter(&mask);
    painter.fillRect(rect, Qt::white);
    painter.setBrush(QColor(0, 0, 0));
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter.drawRoundedRect(rect, ratioX, ratioY);
    return mask;
}

QByteArray DownloadUtils::Widget::getPixmapData(const QPixmap &pix)
{
    QByteArray data;
    QBuffer buffer(&data);
    if(buffer.open(QIODevice::WriteOnly))
    {
        pix.save(&buffer, JPG_FILE_PREFIX);
    }
    buffer.close();
    return data;
}

int DownloadUtils::Widget::reRenderAlpha(int alpha, int value)
{
    return reRenderValue<int>(0xFF, alpha, value);
}

void DownloadUtils::Widget::reRenderImage(int delta, const QImage *input, QImage *output)
{
    for(int w=0; w<input->width(); w++)
    {
        for(int h=0; h<input->height(); h++)
        {
            QRgb rgb = input->pixel(w, h);
            uint resultR = colorBurnTransform(qRed(rgb), delta);
            uint resultG = colorBurnTransform(qGreen(rgb), delta);
            uint resultB = colorBurnTransform(qBlue(rgb), delta);
            uint newRgb = ((resultR & 0xFF)<<16 | (resultG & 0xFF)<<8 | (resultB & 0xFF));
            output->setPixel(w, h, newRgb);
        }
    }
}

void DownloadUtils::Widget::reRenderImage(qint64 &avg, int delta, const QImage *input, QImage *output)
{
   for(int w=0; w<input->width(); w++)
   {
       for(int h=0; h<input->height(); h++)
       {
           QRgb rgb = input->pixel(w, h);
           avg += rgb;
           uint resultR = colorBurnTransform(qRed(rgb), delta);
           uint resultG = colorBurnTransform(qGreen(rgb), delta);
           uint resultB = colorBurnTransform(qBlue(rgb), delta);
           uint newRgb = ((resultR & 0xFF)<<16 | (resultG & 0xFF)<<8 | (resultB & 0xFF));
           output->setPixel(w, h, newRgb);
       }
   }
   avg /= (input->width()*input->height());
}

uint DownloadUtils::Widget::colorBurnTransform(int c, int delta)
{
    Q_ASSERT(0 <= delta && delta < 0xFF);
    if(0 <= delta || delta >= 0xFF)
    {
        return c;
    }

    int result = (c - (uint)(c*delta)/(0xFF - delta));
    if(result > 0xFF)
    {
        result = 0xFF;
    }else if(result < 0)
    {
        result = 0;
    }
    return result;
}

QString DownloadUtils::Widget::getOpenFileDialog(QWidget *obj, const QString &title, const QString &filter)
{
    return QFileDialog::getOpenFileName(obj, title, QDir::currentPath(), filter);
}

QString DownloadUtils::Widget::getOpenFileDialog(QWidget *obj, const QString &filter)
{
    return getOpenFileDialog(obj, QObject::tr("choose a filename to open under"), filter);
}

QString DownloadUtils::Widget::getOpenFileDialog(QWidget *obj)
{
    return getOpenFileDialog(obj, "Images (*.png *.bmp *.jpg)");
}

QStringList DownloadUtils::Widget::getOpenFilesDialog(QWidget *obj, const QString &title, const QString &filter)
{
    return QFileDialog::getOpenFileNames(obj, title, QDir::currentPath(), filter);
}

QStringList DownloadUtils::Widget::getOpenFilesDialog(QWidget *obj, const QString &filter)
{
    return getOpenFilesDialog(obj, QObject::tr("choose a filename to open under"), filter);
}

QStringList DownloadUtils::Widget::getOpenFilesDialog(QWidget *obj)
{
    return getOpenFilesDialog(obj, "Images (*.png *.bmp *.jpg)");
}

QString DownloadUtils::Widget::getSaveFileDialog(QWidget *obj, const QString &title, const QString &filter)
{
    return QFileDialog::getSaveFileName(obj, title, QDir::currentPath(), filter);
}

QString DownloadUtils::Widget::getSaveFileDialog(QWidget *obj, const QString &filter)
{
    return getSaveFileDialog(obj, QObject::tr("choose a filename to save under"), filter);
}

QString DownloadUtils::Widget::getSaveFileDialog(QWidget *obj)
{
    return getSaveFileDialog(obj, "Images (*.png *.bmp *.jpg)");
}
