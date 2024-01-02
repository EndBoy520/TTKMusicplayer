#include "musicsongsearchonlinewidget.h"
#include "musicsongsearchrecordconfigmanager.h"
#include "musicdownloadwidget.h"
#include "musicconnectionpool.h"
#include "musicdownloadqueryfactory.h"
#include "musicrightareawidget.h"
#include "musicgiflabelwidget.h"
#include "musicdownloadbatchwidget.h"
#include "musictoastlabel.h"
#include "musicapplication.h"
#include "musicwidgetheaders.h"

#include <QButtonGroup>

MusicSongSearchTableWidget::MusicSongSearchTableWidget(QWidget *parent)
    : MusicItemSearchTableWidget(parent)
{
    setColumnCount(9);

    QHeaderView *headerview = horizontalHeader();
    headerview->resizeSection(0, 30);
    headerview->resizeSection(1, 273);
    headerview->resizeSection(2, 105);
    headerview->resizeSection(3, 105);
    headerview->resizeSection(4, 55);
    headerview->resizeSection(5, 23);
    headerview->resizeSection(6, 26);
    headerview->resizeSection(7, 26);
    headerview->resizeSection(8, 26);

    G_CONNECTION_PTR->setValue(MusicQueryTableWidget::className(), this);
}

MusicSongSearchTableWidget::~MusicSongSearchTableWidget()
{
    removeItems();
}

void MusicSongSearchTableWidget::startSearchQuery(const QString &text)
{
    if(!G_NETWORK_PTR->isOnline())   //no network connection
    {
        removeItems();
        return;
    }

    MusicSongSearchRecordConfigManager manager;
    if(!manager.fromFile(SEARCH_PATH_FULL))
    {
        return;
    }

    MusicSearchRecordList records;
    manager.readBuffer(records);

    MusicSearchRecord record;
    record.m_name = text;
    record.m_timestamp = QString::number(TTKDateTime::currentTimestamp());
    records.insert(0, record);

    manager.reset();
    manager.writeBuffer(records);

    MusicItemSearchTableWidget::startSearchQuery(text);

    m_loadingLabel->run(true);
    m_networkRequest->setQueryType(MusicAbstractQueryRequest::QueryType::Music);
    m_networkRequest->startToSearch(text);
}

void MusicSongSearchTableWidget::downloadQueryResult(int row)
{
    if(!isValid(row))
    {
        return;
    }

    MusicDownloadWidget *download = new MusicDownloadWidget(this);
    download->setSongName(m_networkRequest, row);
    download->show();
}

void MusicSongSearchTableWidget::startSearchSingleQuery(const QString &text)
{
    if(!G_NETWORK_PTR->isOnline())   //no network connection
    {
        removeItems();
        return;
    }
    //
    MusicItemSearchTableWidget::startSearchQuery(text);
    //
    m_loadingLabel->run(true);
    m_networkRequest->startToSingleSearch(text);
}

void MusicSongSearchTableWidget::resizeSection()
{
    const int width = G_SETTING_PTR->value(MusicSettingManager::WidgetSize).toSize().width();
    QHeaderView *headerview = horizontalHeader();
    headerview->resizeSection(1, 273 + (width - WINDOW_WIDTH_MIN) / 3.0);
    headerview->resizeSection(2, 105 + (width - WINDOW_WIDTH_MIN) / 3.0);
    headerview->resizeSection(3, 105 + (width - WINDOW_WIDTH_MIN) / 3.0);

    for(int i = 0; i < rowCount(); ++i)
    {
        QTableWidgetItem *it = item(i, 1);
        it->setText(TTK::Widget::elidedText(font(), it->toolTip(), Qt::ElideRight, headerview->sectionSize(1) - 31));

        it = item(i, 2);
        it->setText(TTK::Widget::elidedText(font(), it->toolTip(), Qt::ElideRight, headerview->sectionSize(2) - 31));

        it = item(i, 3);
        it->setText(TTK::Widget::elidedText(font(), it->toolTip(), Qt::ElideRight, headerview->sectionSize(3) - 31));
    }
}

void MusicSongSearchTableWidget::itemCellEntered(int row, int column)
{
    MusicItemSearchTableWidget::itemCellEntered(row, column);
    if(column == 7 || column == 8)
    {
        setCursor(QCursor(Qt::PointingHandCursor));
    }
    else
    {
        unsetCursor();
    }
}

void MusicSongSearchTableWidget::itemCellClicked(int row, int column)
{
    MusicItemSearchTableWidget::itemCellClicked(row, column);
    switch(column)
    {
        case 7: addSearchMusicToPlaylist(row, false); break;
        case 8: downloadQueryResult(row); break;
        default: break;
    }
}

void MusicSongSearchTableWidget::itemDoubleClicked(int row, int column)
{
    if(column <= 0 || row < 0 || row >= rowCount() - 1)
    {
        return;
    }

    addSearchMusicToPlaylist(row, true);
}

void MusicSongSearchTableWidget::removeItems()
{
    MusicItemSearchTableWidget::removeItems();
    setColumnCount(9);
}

void MusicSongSearchTableWidget::createSearchedItem(const MusicResultInfoItem &songItem)
{
    const int count = rowCount();
    setRowCount(count + 1);

    QHeaderView *headerview = horizontalHeader();
    QTableWidgetItem *item = new QTableWidgetItem;
    item->setData(TTK_CHECKED_ROLE, Qt::Unchecked);
    setItem(count, 0, item);

                      item = new QTableWidgetItem;
    item->setToolTip(songItem.m_songName);
    item->setText(TTK::Widget::elidedText(font(), item->toolTip(), Qt::ElideRight, headerview->sectionSize(1) - 15));
    item->setForeground(QColor(TTK::UI::Color02));
    setItem(count, 1, item);

                      item = new QTableWidgetItem;
    item->setToolTip(songItem.m_singerName);
    item->setText(TTK::Widget::elidedText(font(), item->toolTip(), Qt::ElideRight, headerview->sectionSize(2) - 30));
    item->setForeground(QColor(TTK::UI::Color02));
    setItem(count, 2, item);

                      item = new QTableWidgetItem;
    item->setToolTip(songItem.m_albumName);
    item->setText(TTK::Widget::elidedText(font(), item->toolTip(), Qt::ElideRight, headerview->sectionSize(3) - 30));
    item->setForeground(QColor(TTK::UI::Color02));
    setItem(count, 3, item);

                      item = new QTableWidgetItem(songItem.m_duration);
    item->setForeground(QColor(TTK::UI::Color02));
    setItem(count, 4, item);

                      item = new QTableWidgetItem;
    item->setIcon(QIcon(randSimulation()));
    setItem(count, 5, item);

                      item = new QTableWidgetItem;
    item->setIcon(QIcon(":/tiny/lb_server_type"));
    item->setToolTip(songItem.m_type);
    setItem(count, 6, item);

                      item = new QTableWidgetItem;
    item->setIcon(QIcon(":/contextMenu/btn_add"));
    setItem(count, 7, item);

                      item = new QTableWidgetItem;
    item->setIcon(QIcon(":/contextMenu/btn_download"));
    setItem(count, 8, item);
}

void MusicSongSearchTableWidget::actionGroupClick(QAction *action)
{
    const int row = currentRow();
    if(!m_networkRequest || row < 0 || row >= rowCount() - 1)
    {
        return;
    }

    const TTK::MusicSongInformationList songInfos(m_networkRequest->songInfoList());
    const TTK::MusicSongInformation &info = songInfos[row];

    switch(action->data().toInt())
    {
        case 0: downloadQueryResult(row); break;
        case 1: Q_EMIT restartSearchQuery(info.m_songName); break;
        case 2: MusicRightAreaWidget::instance()->showArtistFound(info.m_singerName, info.m_artistId); break;
        case 3: Q_EMIT restartSearchQuery(info.m_singerName + " - " + info.m_songName); break;
        case 4: addSearchMusicToPlaylist(row, true); break;
        case 5: MusicRightAreaWidget::instance()->showAlbumFound(info.m_albumName, info.m_albumId); break;
        default: break;
    }
}

void MusicSongSearchTableWidget::contextMenuEvent(QContextMenuEvent *event)
{
    MusicItemSearchTableWidget::contextMenuEvent(event);

    QMenu menu(this);
    m_actionGroup->addAction(menu.addAction(QIcon(":/contextMenu/btn_play"), tr("Play")))->setData(4);

    createContextMenu(menu);

    if(!m_actionGroup->actions().isEmpty())
    {
        const QString &albumName = (currentRow() != -1 && rowCount() > 0) ? item(currentRow(), 3)->toolTip() : QString();
        QAction *lastAction = m_actionGroup->actions().back();
        QAction *action = m_actionGroup->addAction(tr("Search '%1'").arg(albumName));
        action->setData(5);
        menu.insertAction(lastAction, action);
    }
    menu.exec(QCursor::pos());
}

void MusicSongSearchTableWidget::addSearchMusicToPlaylist(int row, bool play)
{
    if(!G_NETWORK_PTR->isOnline())   //no network connection
    {
        return;
    }

    if(!isValid(row))
    {
        MusicToastLabel::popup(tr("Please select one item first"));
        return;
    }

    TTK::MusicSongInformationList songInfos(m_networkRequest->songInfoList());
    TTK::MusicSongInformation &songInfo = songInfos[row];
    m_networkRequest->startToQueryResult(&songInfo, TTK_BN_128);

    if(!songInfo.m_songProps.isEmpty())
    {
        const TTK::MusicSongProperty &prop = songInfo.m_songProps.front();

        MusicResultDataItem result;
        result.m_name = item(row, 2)->toolTip() + " - " + item(row, 1)->toolTip();
        result.m_updateTime = songInfo.m_duration;
        result.m_id = songInfo.m_songId;
        result.m_nickName = prop.m_url;
        result.m_description = prop.m_format;
        result.m_count = prop.m_size;
        result.m_category = play ? MUSIC_PLAY_NOW : MUSIC_PLAY_LATER;

        if(m_networkRequest)
        {
            result.m_id = m_networkRequest->queryServer() + result.m_id;
        }
        Q_EMIT songBufferToPlaylist(result);
    }
}



MusicSongSearchOnlineWidget::MusicSongSearchOnlineWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *boxLayout = new QVBoxLayout(this);
    boxLayout->setContentsMargins(0, 0, 0, 0);
    boxLayout->setSpacing(0);

    QWidget *toolWidget = new QWidget(this);
    toolWidget->setFixedHeight(80);
    QPalette plt(palette());
    plt.setColor(QPalette::Background, Qt::white);
    toolWidget->setAutoFillBackground(true);
    toolWidget->setPalette(plt);

    m_searchTableWidget = new MusicSongSearchTableWidget(this);
    boxLayout->addWidget(toolWidget);
#if !TTK_QT_VERSION_CHECK(5,0,0)
    QWidget *containerWidget = new QWidget(this);
    containerWidget->setAutoFillBackground(true);
    containerWidget->setPalette(plt);

    QVBoxLayout *layout = new QVBoxLayout(containerWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(m_searchTableWidget);
    containerWidget->setLayout(layout);
    boxLayout->addWidget(containerWidget);
#else
    boxLayout->addWidget(m_searchTableWidget);
#endif
    setLayout(boxLayout);

    createToolWidget(toolWidget);
    connect(m_searchTableWidget, SIGNAL(restartSearchQuery(QString)), MusicRightAreaWidget::instance(), SLOT(showSongSearchedFound(QString)));
}

MusicSongSearchOnlineWidget::~MusicSongSearchOnlineWidget()
{
    delete m_playButton;
    delete m_textLabel;
    delete m_searchTableWidget;
}

void MusicSongSearchOnlineWidget::startSearchQuery(const QString &name)
{
    setResizeLabelText(name);
    if(!m_resizeWidgets.isEmpty())
    {
        TTKObjectCast(QCheckBox*, m_resizeWidgets[0])->setChecked(false);
    }

    m_searchTableWidget->startSearchQuery(name);
}

void MusicSongSearchOnlineWidget::startSearchSingleQuery(const QString &name)
{
    setResizeLabelText(name);
    if(!m_resizeWidgets.isEmpty())
    {
        TTKObjectCast(QCheckBox*, m_resizeWidgets[0])->setChecked(false);
    }

    m_searchTableWidget->startSearchSingleQuery(name);
}

void MusicSongSearchOnlineWidget::resizeWindow()
{
    setResizeLabelText(m_textLabel->toolTip());
    m_searchTableWidget->resizeSection();
}

void MusicSongSearchOnlineWidget::buttonClicked(int index)
{
    TTKIntList list = m_searchTableWidget->checkedIndexList();
    list.removeOne(m_searchTableWidget->rowCount() - 1);

    if(list.isEmpty())
    {
        MusicToastLabel::popup(tr("Please select one item first"));
        return;
    }

    if(index == 0)
    {
        for(int i = 0; i < list.count(); ++i)
        {
            m_searchTableWidget->addSearchMusicToPlaylist(list[i], i == 0);
        }
    }
    else if(index == 1)
    {
        for(int row : qAsConst(list))
        {
            m_searchTableWidget->addSearchMusicToPlaylist(row, false);
        }
    }
    else if(index == 2)
    {
        MusicAbstractQueryRequest *d = m_searchTableWidget->queryInput();
        if(!d)
        {
            return;
        }

        MusicDownloadBatchWidget w;
        w.setSongName(d, list);
        w.exec();
    }
}

void MusicSongSearchOnlineWidget::createToolWidget(QWidget *widget)
{
    QVBoxLayout *wLayout = new QVBoxLayout(widget);
    wLayout->setContentsMargins(0, 0, 0, 0);
    wLayout->setSpacing(0);
    widget->setLayout(wLayout);

    QWidget *funcWidget = new QWidget(widget);
    QHBoxLayout *funcLayout = new QHBoxLayout(funcWidget);
    funcLayout->setContentsMargins(0, 0, 10, 0);
    funcLayout->setSpacing(10);

    m_textLabel = new QLabel(this);
    m_textLabel->setTextFormat(Qt::RichText);
    m_textLabel->setText(tr("&nbsp;Find no result"));
    m_textLabel->setStyleSheet(TTK::UI::ColorStyle03);
    funcLayout->addWidget(m_textLabel);

    m_playButton = new QPushButton(tr("Play"), this);
    m_playButton->setIcon(QIcon(":/contextMenu/btn_play_white"));
    m_playButton->setIconSize(QSize(14, 14));
    m_playButton->setFixedSize(70, 25);
    m_playButton->setStyleSheet(TTK::UI::PushButtonStyle03);
    m_playButton->setCursor(QCursor(Qt::PointingHandCursor));
    funcLayout->addWidget(m_playButton);

    QPushButton *addButton = new QPushButton(tr("Add"), this);
    addButton->setFixedSize(70, 25);
    addButton->setStyleSheet(TTK::UI::PushButtonStyle03);
    addButton->setCursor(QCursor(Qt::PointingHandCursor));
    funcLayout->addWidget(addButton);

    QPushButton *downloadButton = new QPushButton(tr("Download"), this);
    downloadButton->setFixedSize(70, 25);
    downloadButton->setStyleSheet(TTK::UI::PushButtonStyle03);
    downloadButton->setCursor(QCursor(Qt::PointingHandCursor));
    funcLayout->addWidget(downloadButton);

    QButtonGroup *buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(m_playButton, 0);
    buttonGroup->addButton(addButton, 1);
    buttonGroup->addButton(downloadButton, 2);
    QtButtonGroupConnect(buttonGroup, this, buttonClicked, TTK_SLOT);
    funcWidget->setLayout(funcLayout);
    wLayout->addWidget(funcWidget);

    QHeaderView *headerview = m_searchTableWidget->horizontalHeader();
    //
    QWidget *labelWidget = new QWidget(widget);
    QHBoxLayout *labelLayout = new QHBoxLayout(labelWidget);
    labelLayout->setContentsMargins(7, 0, 10, 0);
    labelLayout->setSpacing(0);
    labelWidget->setStyleSheet(TTK::UI::BackgroundStyle09);

    QCheckBox *labelCheckBox = new QCheckBox(this);
    labelCheckBox->setStyleSheet(TTK::UI::CheckBoxStyle01);
    connect(labelCheckBox, SIGNAL(clicked(bool)), m_searchTableWidget, SLOT(checkedItemsState(bool)));
    labelLayout->addWidget(labelCheckBox, headerview->sectionSize(0));
    m_resizeWidgets << labelCheckBox;

#ifdef Q_OS_UNIX
    m_playButton->setFocusPolicy(Qt::NoFocus);
    addButton->setFocusPolicy(Qt::NoFocus);
    downloadButton->setFocusPolicy(Qt::NoFocus);
    labelCheckBox->setFocusPolicy(Qt::NoFocus);
#endif

    QLabel *label1 = new QLabel(tr("Song"), this);
    label1->setStyleSheet(TTK::UI::FontStyle01);
    labelLayout->addWidget(label1, headerview->sectionSize(1));
    m_resizeWidgets << label1;

    QLabel *label2 = new QLabel(tr("Artist"), this);
    label2->setStyleSheet(TTK::UI::FontStyle01);
    labelLayout->addWidget(label2, headerview->sectionSize(2));
    m_resizeWidgets << label2;

    QLabel *label3 = new QLabel(tr("Album"), this);
    label3->setStyleSheet(TTK::UI::FontStyle01);
    labelLayout->addWidget(label3, headerview->sectionSize(3));
    m_resizeWidgets << label3;

    QLabel *label4 = new QLabel(tr("Operator"), this);
    label4->setStyleSheet(TTK::UI::FontStyle01);
    labelLayout->addWidget(label4, headerview->sectionSize(4) + headerview->sectionSize(5) + headerview->sectionSize(6) * 3);
    m_resizeWidgets << label3;

    labelWidget->setLayout(labelLayout);
    wLayout->addWidget(labelWidget);
}

void MusicSongSearchOnlineWidget::setResizeLabelText(const QString &name)
{
    const int width = G_SETTING_PTR->value(MusicSettingManager::WidgetSize).toSize().width();
    if(!m_resizeWidgets.isEmpty())
    {
        m_resizeWidgets[1]->setFixedWidth((width - WINDOW_WIDTH_MIN) / 3.0 + 273);
        m_resizeWidgets[2]->setFixedWidth((width - WINDOW_WIDTH_MIN) / 3.0 + 105);
        m_resizeWidgets[3]->setFixedWidth((width - WINDOW_WIDTH_MIN) / 3.0 + 105);
    }

    m_textLabel->setText(tr("&nbsp;find <font color=#158FE1> %1 </font> result").arg(TTK::Widget::elidedText(font(), name, Qt::ElideRight, width - WINDOW_WIDTH_MIN + 240)));
    m_textLabel->setToolTip(name);
}
