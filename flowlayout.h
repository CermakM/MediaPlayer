#ifndef FLOWLAYOUT_H
#define FLOWLAYOUT_H

#include <QLayout>
#include <QWidget>
#include <QRect>
#include <QStyle>

class FlowLayout : public QLayout
{
public:

    explicit FlowLayout(QWidget *parent, int margin = -1, int hSpacing = -1, int vSpacing = -1);
    explicit FlowLayout(int margin = -1, int hSpacing = -1, int vSpacing = -1);
    ~FlowLayout();

    void addItem(QLayoutItem *item) override;

    int count() const override;

    QLayoutItem* itemAt(int index) const override;

    QLayoutItem* takeAt(int index) override;

    int horizontalSpacing() const;

    int verticalSpacing() const;

    Qt::Orientations expandingDirections() const;

    bool hasHeightForWidth() const;

    int  heightForWidth(int width) const;

    void setGeometry(const QRect &rect);

    QSize sizeHint() const;

    QSize minimumSize() const;

protected:

    int doLayout(const QRect &rect) const;

    int smartSpacing(QStyle::PixelMetric pixel_metric) const;

private:

    QList<QLayoutItem *> _itemList;
    int _hSpacing;
    int _vSpacing;
};

#endif // FLOWLAYOUT_H
