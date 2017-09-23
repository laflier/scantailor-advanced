/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C)  Joseph Artsimovich <joseph.artsimovich@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "RelinkingListView.h"
#include "RelinkingModel.h"
#include <QStyledItemDelegate>
#include <QPainter>

class RelinkingListView::Delegate: public QStyledItemDelegate {
public:
    Delegate(RelinkingListView* owner)
            : QStyledItemDelegate(owner),
              m_pOwner(owner) {
    }

    virtual void paint(QPainter* painter, QStyleOptionViewItem const& option, QModelIndex const& index) const {
        m_pOwner->maybeDrawStatusLayer(painter, index, option.rect);
        QStyledItemDelegate::paint(painter, option, index);
    }

private:
    RelinkingListView* m_pOwner;
};


class RelinkingListView::IndicationGroup {
public:
    QRect rect;
    int status;

    IndicationGroup(QRect const& r, int st)
            : rect(r),
              status(st) {
    }
};


class RelinkingListView::GroupAggregator {
public:
    void process(QRect const& rect, int status);

    std::vector<IndicationGroup> const& groups() const {
        return m_groups;
    }

private:
    std::vector<IndicationGroup> m_groups;
};


RelinkingListView::RelinkingListView(QWidget* parent)
        : QListView(parent),
          m_statusLayerDrawn(false) {
    setItemDelegate(new Delegate(this));
}

void RelinkingListView::paintEvent(QPaintEvent* e) {
    m_statusLayerDrawn = false;
    QListView::paintEvent(e);
}

void RelinkingListView::maybeDrawStatusLayer(QPainter* painter,
                                             QModelIndex const& item_index,
                                             QRect const& item_paint_rect) {
    if (m_statusLayerDrawn) {
        return;
    }

    painter->save();
    painter->translate(item_paint_rect.topLeft() - visualRect(item_index).topLeft());
    painter->setClipRect(viewport()->rect());
    drawStatusLayer(painter);
    painter->restore();

    m_statusLayerDrawn = true;
}

void RelinkingListView::drawStatusLayer(QPainter* painter) {
    QRect const drawing_rect(viewport()->rect());
    QModelIndex top_index(this->indexAt(drawing_rect.topLeft()));
    if (!top_index.isValid()) {
        return;
    }

    if (top_index.row() > 0) {
        top_index = top_index.sibling(top_index.row() - 1, 0);
    }

    GroupAggregator group_aggregator;
    int const rows = top_index.model()->rowCount(top_index.parent());

    for (int row = top_index.row(); row < rows; ++row) {
        QModelIndex const index(top_index.sibling(row, 0));
        QRect const item_rect(visualRect(index));

        QRect rect(drawing_rect);
        rect.setTop(item_rect.top());
        rect.setBottom(item_rect.bottom());
        rect.setWidth(item_rect.height());
        rect.moveRight(drawing_rect.right());

        int const status = index.data(RelinkingModel::UncommittedStatusRole).toInt();
        group_aggregator.process(rect, status);

        if ((row != top_index.row()) && !item_rect.intersects(drawing_rect)) {
            break;
        }
    }

    painter->setRenderHint(QPainter::Antialiasing);

    QPen pen(QColor(0x3a5827));
    pen.setWidthF(1.5);
    QBrush brush(QColor(0x89e74a));

    for (int status = RelinkingModel::Exists, i = 0; i < 2; ++i) {
        painter->setPen(pen);
        painter->setBrush(brush);

        for (IndicationGroup const& group : group_aggregator.groups()) {
            if (group.status == status) {
                qreal const radius = 0.5 * group.rect.width();
                QRectF rect(group.rect);
                rect.adjust(pen.widthF(), pen.widthF(), -pen.widthF(), -pen.widthF());
                painter->drawRoundedRect(rect, radius, radius);
            }
        }

        status = RelinkingModel::Missing;
        pen.setColor(QColor(0x6f2719));
        brush.setColor(QColor(0xff674b));
    }
}  // RelinkingListView::drawStatusLayer

void RelinkingListView::GroupAggregator::process(QRect const& rect, int status) {
    if (m_groups.empty() || (m_groups.back().status != status)) {
        m_groups.push_back(IndicationGroup(rect, status));
    } else {
        m_groups.back().rect |= rect;
    }
}

