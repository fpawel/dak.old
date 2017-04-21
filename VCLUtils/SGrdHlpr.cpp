//------------------------------------------------------------------------------
#include <Map>
//------------------------------------------------------------------------------
#pragma hdrstop
//------------------------------------------------------------------------------
#include "SGrdHlpr.h"
//------------------------------------------------------------------------------
#include <Clipbrd.hpp>
//------------------------------------------------------------------------------
#include "loki\singleton.h"
//------------------------------------------------------------------------------
#include "myformat.hpp"
#include "uFormGridColsDlg.h"
#include "MyVclUtils_.h"
#include "DrawGridCell.h"
#include "AnsiStringUtils_.h"
//------------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------
namespace my
{

namespace Grd
{
	// ВЫбран ли диапазон
    bool IsRangeSelected( const TStringGrid* g )
    {
    	#pragma warn -8037
    	return g->Selection.Top!=g->Selection.Bottom || g->Selection.Left!=g->Selection.Right;
        #pragma warn +8037
    }

    void SetTxt(GridCell& cell, const AnsiString& s)
    {
    	cell.first->Cells[cell.second.first][cell.second.second] = s;
    }
    AnsiString GetTxt(const GridCell& cell)
    {
    	return cell.first->Cells[cell.second.first][cell.second.second];
    }

    //---------------------------------------------------------------------------
	// Попадает ли ячейка col,row в фиксированную область таблицы
    bool IsCellFixed(const GridCell& cell)
	{
		return cell.second.first < cell.first->FixedCols || cell.second.second < cell.first->FixedRows;
	}
    //---------------------------------------------------------------------------
    bool IsCellInEditMode( const GridCell& cell )
	{
		return 	cell.first->EditorMode &&
        		cell.first->Col == cell.second.first &&
                cell.first->Row == cell.second.second;
	}
	//---------------------------------------------------------------------------
    void SetCellIfNotEdited(GridCell cell, AnsiString val)
	{
		if( !IsCellInEditMode(cell) )
        	SetTxt(cell, val);
	}
    //---------------------------------------------------------------------------
    // Отрисовать ячейку col,row в таблице g
    void RedrawCell(TStringGrid* grd, int col, int row)
    {
    	RedrawCell( GridCell( grd, std::make_pair(col,row) ) );
    }

	void RedrawCell(GridCell cell)
	{
		SetCellIfNotEdited( cell, GetTxt(cell) );
	}

    void RedrawRow(TStringGrid* grd, int row)
    {
    #pragma warn -8012
    	for( unsigned cl=0; cl<grd->ColCount; ++cl )
        	RedrawCell(grd, cl, row);
    #pragma warn +8012
    }


    void RedrawCol(TStringGrid* grd, int col)
    {
    	for( int ro=0; ro<grd->RowCount; ++ro )
        	RedrawCell(grd, col, ro);
    }

    void RedrawGrid(TStringGrid* grd )
    {
    #pragma warn -8012
    	for( unsigned ro=0; ro<grd->RowCount; ++ro )
    	for( unsigned cl=0; cl<grd->ColCount; ++cl )
        	RedrawCell(grd, cl, ro);
    #pragma warn +8012
    }

	// Возвращает видимый прямоугольник нефиксированных ячеек таблицы
	TRect VisRect(TStringGrid* g)
	{
		int topRow = g->TopRow, visRows = g->VisibleRowCount,
			leftCol = g->LeftCol, visCols = g->VisibleColCount;
		return TRect( leftCol, topRow, leftCol+visCols, topRow+visRows );
	}

	//---------------------------------------------------------------------------
	// сделать таблицу редактируемой или нет
	void SetEditing(TStringGrid* grd, bool fl)
	{
		if (fl) grd->Options<<goEditing ;
		else grd->Options>>goEditing;
	}
	//---------------------------------------------------------------------------
	// заполнена ли строка
	bool FilledRow( TStringGrid* grd, int row)
	{
		int colCount = grd->ColCount;
		for (int i=0; i<colCount; ++i)
		if (grd->Cells[i][row]=="") return false;
		return true;
	}
	//---------------------------------------------------------------------------
	// ячейка под курсором
	TPoint HotCell(TStringGrid* grd)
	{
		TPoint pt;
		GetCursorPos(&pt);
		TPoint point = grd->ScreenToClient( pt );

        int col,row;
		grd->MouseToCell(point.x, point.y, col, row);
		return TPoint(col,row);
	}
	//---------------------------------------------------------------------------





	// Отрисовать прямоугольник left, top, right, bottom в таблице g
	void RedrawRect(TStringGrid* grd, TRect rect)
	{
    	const int
        minCol = std::max(0,rect.left),
        minRow = std::max(0,rect.top),
        maxCol = std::min(grd->ColCount-1,rect.right),
        maxRow = std::min(grd->RowCount-1,rect.bottom);

    	GridCell cell( grd, std::make_pair(minCol,minRow) );
		for ( cell.second.first = minCol; cell.second.first <= maxCol; ++cell.second.first )
        	for ( cell.second.second=minRow; cell.second.second <= maxRow; ++cell.second.second)
            	RedrawCell(cell);

	}
	//---------------------------------------------------------------------------

	void ClearTStringGrid( TStringGrid* grd, int cl0, int ro0 )
	{
		const int cc = grd->ColCount, rc = grd->RowCount;
		for( int cl = cl0; cl<cc; ++cl ) for( int ro = ro0; ro<rc; ++ro )
			grd->Cells[cl][ro] = "";
	}
    #pragma warn -8037
    void ClearSelection(const TStringGrid* grd)
    {
    	const unsigned
        minCol = grd->Selection.Left,		maxCol = grd->Selection.Right,
  		minRow = grd->Selection.Top,  		maxRow = grd->Selection.Bottom;

    	AnsiString sBuff;
        for( unsigned ro=minRow; ro<=maxRow; ++ro ) if( grd->RowHeights[ro] )
        {
        	for( unsigned cl=minCol; cl<=maxCol; ++cl ) if (grd->ColWidths[cl]>0)
            	grd->Cells[cl][ro] = "";
        }
    }

    AnsiString GetErasedLastChar( const AnsiString& ss, char ch )
    {
        const int len = ss.Length();
        if( len==0 ) return ss;
        return ( ss[len]==ch ) ? ss.SubString(1,len-1) : ss;
    }

    AnsiString GetFormatedVisibleText( const TStringGrid* grd,
    	unsigned col_from, unsigned col_to,
        unsigned row_from, unsigned row_to)
    {
    	AnsiString s;
  		for ( unsigned ro=row_from; ro<=row_to; ++ro ) if (grd->RowHeights[ro]>0)
        {
        	for ( unsigned cl=col_from; cl<=col_to; ++cl )
            	if (grd->ColWidths[cl]>0) s = s + grd->Cells[cl][ro]+"\t";
            s = GetErasedLastChar( s, '\t' ) + "\n";
        }

  		return s = GetErasedLastChar( s, '\n' );

    }

//------------------------------------------------------------------------------
// вызвать диалог для настройуи видимых столбцов
//------------------------------------------------------------------------------
void SetupVisibleColsOnMouseDownRow0(TObject *Sender, TMouseButton Button,
	TShiftState Shift, int X, int Y, unsigned titleNum)
{
	TStringGrid* grd = static_cast<TStringGrid*>(Sender);
    // ячейка под курсором
	const TPoint hotCell = my::Grd::HotCell(grd);
	if( Button==mbLeft && hotCell.y==0 )
    {
        boost::shared_ptr<TFormGridColsDlg> dlg( new TFormGridColsDlg(NULL, grd, true, titleNum) );
        dlg->ShowModal();
    }
}
//------------------------------------------------------------------------------
void SetupVisibleRows(TStringGrid* grd, unsigned titleNum)
{
	boost::shared_ptr<TFormGridColsDlg> dlg( new TFormGridColsDlg(NULL, grd, false, titleNum) );
    dlg->ShowModal();
}


void SetupVisibleRowsOnMouseDownCol0(TObject *Sender, TMouseButton Button,
	TShiftState Shift, int X, int Y, unsigned titleNum)
{
    TStringGrid* grd = static_cast<TStringGrid*>(Sender);
    // ячейка под курсором
	const TPoint hotCell = my::Grd::HotCell(grd);
	if( Button==mbLeft && hotCell.x==0 )
    {
        boost::shared_ptr<TFormGridColsDlg> dlg( new TFormGridColsDlg(NULL, grd, false, titleNum) );
        dlg->ShowModal();
    }
}

//------------------------------------------------------------------------------
	bool GetColVis( const TStringGrid* grd, unsigned col )
    {
    	return grd->ColWidths[col]!=0;
    }
    void SetColVis(const TStringGrid* grd, unsigned col, bool vis)
    {
    	if( vis!=GetColVis(grd,col) )
    	grd->ColWidths[col] = vis ? grd->DefaultColWidth : 0;
    }
//---------------------------------------------------------------------------
    void SetRowVis(const TStringGrid* grd, unsigned row, bool vis)
    {
    	grd->RowHeights[row]  = vis ? grd->DefaultRowHeight : 0;
    }
//---------------------------------------------------------------------------
    AnsiString GetVisColsInfo(TStringGrid* grd)
    {
    	AnsiString ret;
        const AnsiString sTrue = "1", sFalse = "0";
    	for( int cl=0; cl<grd->ColCount; ++cl )
        	ret = ret + ( grd->ColWidths[cl] ? sTrue : sFalse );
        return ret;
    }
//---------------------------------------------------------------------------
    void SetVisColsInfo(TStringGrid* grd, const AnsiString& s)
    {
    	const int len = s.Length(), clCnt = grd->ColCount, vv = std::min(len, clCnt);
    	for( int cl=0; cl<vv; ++cl )
        {
        	const bool vis = s[cl+1]!='0';
        	SetColVis(grd, cl, vis);
        }
    }
//---------------------------------------------------------------------------
    AnsiString GetVisRowsInfo(TStringGrid* grd)
    {
    	AnsiString ret;
        const AnsiString sTrue = "1", sFalse = "0";
    	for( int ro=0; ro<grd->RowCount; ++ro )
        	ret = ret + ( grd->RowHeights[ro] ? sTrue : sFalse );
        return ret;
    }
//---------------------------------------------------------------------------
    void SetVisRowsInfo(TStringGrid* grd, const AnsiString& s)
    {
    	const int len = s.Length(), roCnt = grd->RowCount, vv = std::min(len, roCnt);
    	for( int ro=0; ro<vv; ++ro )
        {
        	const bool vis = s[ro+1]!='0';
        	SetRowVis(grd, ro, vis);
        }
    }
 //---------------------------------------------------------------------------
    // копировать в буфер
    void CopyCellsToClipboard( const TStringGrid* grd )
    {
    	const unsigned
        minCol = grd->Selection.Left,		maxCol = grd->Selection.Right,
  		minRow = grd->Selection.Top,  		maxRow = grd->Selection.Bottom;

        MyCopyTextToClipboard( GetFormatedVisibleText(grd, minCol, maxCol, minRow, maxRow) );
        /*
        const WideString wsGrdContent = sGrdContent;

        const unsigned
        	strLen = wsGrdContent.Length(),
        	buffSize = (strLen+1)*sizeof(wchar_t);

        HGLOBAL hMem = ::GlobalAlloc(GMEM_MOVEABLE,  buffSize );

        wchar_t *pOut = (wchar_t*) ::GlobalLock(hMem), *pIn = wsGrdContent.c_bstr();
        std::copy(pIn, pIn + strLen, pOut);
        pOut[strLen] = L'\0';

        ::GlobalUnlock(hMem);
        ::OpenClipboard( NULL );
        ::EmptyClipboard();
        ::SetClipboardData( CF_UNICODETEXT, hMem);

        ::CloseClipboard();
        */
    }
//---------------------------------------------------------------------------
    typedef std::vector<AnsiString> MyStringList;
    void GetSubStrsDelimitedByOneOf(MyStringList& v, const std::string& s,
        const std::vector< std::string>& delims)
    {
        using std::string::npos;
	    unsigned p1 = 0, p2 = 0;
	    while( p2 != npos )
	    {
            unsigned p3 = npos;
            for( unsigned i=0; p3==npos && i<delims.size(); ++i )
                p3 = s.find(delims[i], p1);
		    p2 = p3;
            const std::string toPushBack = s.substr( p1, p2-p1 );
		    v.push_back( toPushBack.c_str() );
		    p1 = p2+1;
	    }
    }

//------------------------------------------------------------------------------
class ReadOnlyCellsT : public boost::noncopyable
	{
    public:
    	explicit ReadOnlyCellsT() {}
        bool Is( GridCell cell )
        {
        	return
            	std::find( skipCols_.begin(), skipCols_.end(),
                	GridCol( cell.first, cell.second.first ) )!=skipCols_.end()
                    ||
                std::find( skipRows_.begin(), skipRows_.end(),
                	GridCol( cell.first, cell.second.second ) )!=skipRows_.end()
                    ||
                std::find( skipCells_.begin(), skipCells_.end(), cell ) != skipCells_.end()
            ;
        }
        void AddCol(TStringGrid* grd, int col)
    	{
    		skipCols_.push_back( GridCol(grd,col) );
            RegisterGrid(grd);
    	}
    	void AddRow(TStringGrid* grd, int row)
    	{
    		skipRows_.push_back( GridRow(grd,row) );
            RegisterGrid(grd);
    	}
    private:
    	std::vector<GridCell> skipCells_;
        std::vector<GridCol> skipCols_;
        std::vector<GridRow> skipRows_;

        typedef std::map<TStringGrid*, TSelectCellEvent> Grd2OnSelectCellMapT;
        Grd2OnSelectCellMapT grd2OnSelectCell_;

        void RegisterGrid(TStringGrid* grd)
        {
        	Grd2OnSelectCellMapT::const_iterator iOnSelectCell =  grd2OnSelectCell_.find(grd);
            if( iOnSelectCell==grd2OnSelectCell_.end() )
            {
            	grd2OnSelectCell_[grd] = grd->OnSelectCell;
                grd->OnSelectCell = &GrdSelectCell;
        	}
        }

        void __fastcall GrdSelectCell(TObject *Sender, int ACol,
      		int ARow, bool &CanSelect)
		{
        	TStringGrid* grd = static_cast<TStringGrid*>(Sender);
            Grd2OnSelectCellMapT::const_iterator iOnSelectCell =  grd2OnSelectCell_.find(grd);
            my::Grd::SetEditing( grd, !this->Is( GridCellT(grd, ACol, ARow) ) );
            if( iOnSelectCell!=grd2OnSelectCell_.end() && iOnSelectCell->second )
            	iOnSelectCell->second(Sender, ACol, ARow, CanSelect);

        }
	};
//------------------------------------------------------------------------------
	typedef Loki::SingletonHolder
	<
    	ReadOnlyCellsT,
    	Loki::CreateUsingNew,
    	Loki::NoDestroy
	> ReadOnlyCells;
//------------------------------------------------------------------------------
    void AddSkipPastCol(TStringGrid* grd, int col)
    {
    	ReadOnlyCells::Instance().AddCol(grd,col);
    }
    void AddSkipPastRow(TStringGrid* grd, int row)
    {
    	ReadOnlyCells::Instance().AddRow(grd,row);
    }
//------------------------------------------------------------------------------
    typedef std::vector<MyStringList> MyStringLists;
    MyStringLists FormatClipboard()
    {
        MyStringLists ret;
        TClipboard& clpBrd = *Clipboard();
    	if ( !clpBrd.HasFormat(CF_TEXT) ) return ret;
        MyStringList lines;
        std::vector< std::string> delims;
        delims.push_back("\n");
        delims.push_back("\r\n");
        delims.push_back("\r");
        GetSubStrsDelimitedByOneOf(lines, clpBrd.AsText.c_str(), delims);
        delims.clear();
        delims.push_back("\t");
        for( unsigned i=0; i<lines.size(); ++i )
        {
            MyStringList line;
            GetSubStrsDelimitedByOneOf(line, lines[i].c_str(), delims);
            ret.push_back(line);
        }
        return ret;
    }


    void PastCellsFromClipboard(TStringGrid* grd)
    {
    	TClipboard& clpBrd = *Clipboard();
    	if ( !clpBrd.HasFormat(CF_TEXT) ) return;
        const unsigned roCnt = (unsigned)grd->RowCount, clCnt = (unsigned)grd->ColCount;

    	boost::shared_ptr<TStringList> tmpLst( new TStringList ), sL( new TStringList );
        sL->Text = clpBrd.AsText;
        unsigned ro = grd->Row;
        for( int i=0; i<sL->Count; ++i )
        {
        	while (grd->RowHeights[ro]==0 && ro<roCnt) ++ro;
      		if (ro>=roCnt) break;
            {
            AnsiString s = sL->Strings[i];
            for ( int idxS=1; idxS<=s.Length(); ++idxS )
                if ( s[idxS]=='\t' ) s[idxS]='\n';
    		tmpLst->Text = s;
            }
            unsigned cl = (unsigned)grd->Col;
    		for ( int tmpLstIdx=0; tmpLstIdx<tmpLst->Count; ++tmpLstIdx)
    		{
        		while ( grd->ColWidths[cl]==0 && cl<clCnt) ++cl;
        		if (cl >= clCnt) break;
                if( !ReadOnlyCells::Instance().Is( GridCellT( grd,cl,ro) ) )
        			grd->Cells[cl][ro] = tmpLst->Strings[tmpLstIdx];
        		++cl;
    		}
    		++ro;
        }
    }

    #pragma warn +8037
//---------------------------------------------------------------------------
struct MenuToGrdMapT
{
    std::map<TMenuItem*,TStringGrid*> menuToGrd_;
};
//---------------------------------------------------------------------------
typedef Loki::SingletonHolder
<
    MenuToGrdMapT,
    Loki::CreateUsingNew,
    Loki::NoDestroy
> MenuToGrdMap;
//---------------------------------------------------------------------------
struct PMRowsHlprT
{
    std::map<TPopupMenu*,TNotifyEvent> pmToOnPopup_;
    void __fastcall OnClick(TObject* sender)
    {
        TMenuItem* itm = static_cast<TMenuItem*>(sender);
        const int idx = itm->MenuIndex;
        assert(idx>-1);
        TStringGrid *grd = MenuToGrdMap::Instance().menuToGrd_[itm];
        assert(grd);
        itm->Checked = !itm->Checked;
        my::Grd::SetRowVis(grd, idx, itm->Checked);
    }

    void __fastcall OnPopup(TObject* sender)
    {
        TPopupMenu* pm = static_cast<TPopupMenu*>(sender);
    	TStringGrid *grd = static_cast<TStringGrid*>( pm->PopupComponent );
    	for( int ro=0; ro<grd->RowCount; ++ro )
            pm->Items->Items[ro]->Checked = grd->RowHeights[ro];
        TNotifyEvent onPopup = pmToOnPopup_[pm];
        if(onPopup) onPopup(sender);
    }
};


struct PMColsHlprT
{
    std::map<TPopupMenu*,TNotifyEvent> pmToOnPopup_;

    void __fastcall SelectAll(TObject* sender)
    {
        TMenuItem* itm = static_cast<TMenuItem*>(sender);
        TStringGrid *grd = MenuToGrdMap::Instance().menuToGrd_[itm];
        itm->Checked = !itm->Checked;
        for( int cl=0; cl<grd->ColCount; ++cl )
            my::Grd::SetColVis(grd, cl, itm->Checked);
    }

    void __fastcall OnClick(TObject* sender)
    {
        TMenuItem* itm = static_cast<TMenuItem*>(sender);
        const int idx = itm->MenuIndex;
        assert(idx>-1);
        TStringGrid *grd = MenuToGrdMap::Instance().menuToGrd_[itm];
        assert(grd);
        itm->Checked = !itm->Checked;
        my::Grd::SetColVis(grd, idx, itm->Checked);
    }
    void __fastcall OnPopup(TObject* sender)
    {
        TPopupMenu* pm = static_cast<TPopupMenu*>(sender);
    	TStringGrid *grd = static_cast<TStringGrid*>( pm->PopupComponent );
        bool allVis = true;
    	for( int cl=0; cl<grd->ColCount; ++cl )
        {
            pm->Items->Items[cl]->Checked = grd->ColWidths[cl];
            allVis &= pm->Items->Items[cl]->Checked;
        }
        pm->Items->Items[grd->ColCount+1]->Checked = allVis;

        TNotifyEvent onPopup = pmToOnPopup_[pm];
        if(onPopup) onPopup(sender);
    }
};

struct PMCopyHlpr
{
    void __fastcall OnClick(TObject* sender)
    {
        TMenuItem* itm = static_cast<TMenuItem*>(sender);
        TStringGrid *grd = MenuToGrdMap::Instance().menuToGrd_[itm];
        assert(grd);
        CopyCellsToClipboard( grd );
    }
};
struct PMPastHlpr
{
    void __fastcall OnClick(TObject* sender)
    {
        TMenuItem* itm = static_cast<TMenuItem*>(sender);
        TStringGrid *grd = MenuToGrdMap::Instance().menuToGrd_[itm];
        assert(grd);
        PastCellsFromClipboard( grd );
    }
};
//---------------------------------------------------------------------------
void TieWithColsVisiblePopup( TStringGrid* grd, TNotifyEvent onPopup )
{
    static PMColsHlprT hndl_;

    TPopupMenu* pm = GetPopupMenu( grd );
    pm->OnPopup = hndl_.OnPopup;
    for( int cl=0; cl<grd->ColCount; ++cl )
    {
        TMenuItem *itm = new TMenuItem(pm);
    	pm->Items->Add( itm );
        itm->Caption = grd->Cells[cl][0];
        itm->OnClick =  hndl_.OnClick;
        MenuToGrdMap::Instance().menuToGrd_[itm] = grd;
    }

    TMenuItem *itm = new TMenuItem(pm);
    pm->Items->Add( itm );
    itm->Caption = "-";

    itm = new TMenuItem(pm);
    pm->Items->Add( itm );
    itm->Caption = "Показать все";
    itm->OnClick =  hndl_.SelectAll;
    MenuToGrdMap::Instance().menuToGrd_[itm] = grd;

    hndl_.pmToOnPopup_[pm] = onPopup;
}
//---------------------------------------------------------------------------
void TieWithRowsVisiblePopup( TStringGrid* grd, TNotifyEvent onPopup )
{
    static PMRowsHlprT hndl_;

    TPopupMenu* pm = GetPopupMenu( grd );
    pm->OnPopup = hndl_.OnPopup;
    for( int ro=0; ro<grd->RowCount; ++ro )
    {
        	TMenuItem *itm = new TMenuItem(pm);
    		pm->Items->Add( itm );
        	itm->Caption = grd->Cells[0][ro];
            itm->OnClick =  hndl_.OnClick;
            MenuToGrdMap::Instance().menuToGrd_[itm] = grd;
    }
    hndl_.pmToOnPopup_[pm] = onPopup;
}
//---------------------------------------------------------------------------
    void DeleteRow(TStringGrid* grd, int row, int count)
    {
    	const int oldRC = grd->RowCount, cC = grd->ColCount;

        for( int ro=row; ro<oldRC; ++ro )
        	for( int cl=0; cl<cC; ++cl )
            	grd->Cells[cl][ro] = grd->Cells[cl][ro + count];

        for( int ro=oldRC-count; ro<oldRC; ++ro )
        	for( int cl=0; cl<cC; ++cl )
            	grd->Cells[cl][ro] = "";
        grd->RowCount = grd->RowCount - count;
    }
    void DeleteCol(TStringGrid* grd, int col, int count)
    {
    	const int oldCC = grd->ColCount, rC = grd->RowCount;

        for( int cl=col; cl<oldCC; ++cl )
        	for( int ro=0; ro<rC; ++ro )
            	grd->Cells[cl][ro] = grd->Cells[cl+count][ro];

        for( int cl=oldCC-count; cl<oldCC; ++cl )
        	for( int ro=0; ro<rC; ++ro )
            	grd->Cells[cl][ro] = "";
        grd->ColCount = grd->ColCount - count;
    }
//---------------------------------------------------------------------------
void SetCellByTtl(TStringGrid *grd, const AnsiString& ttl, int row, const AnsiString& s)
{
    const int col = grd->Rows[0]->IndexOf(ttl);
    if(col>-1) grd->Cells[col][row] = s;
}
//---------------------------------------------------------------------------
AnsiString GetCellByTtl(TStringGrid *grd, const AnsiString& ttl, int row)
{
    const int col = grd->Rows[0]->IndexOf(ttl);
    return col>-1 ? grd->Cells[col][row] : AnsiString("");
}
//---------------------------------------------------------------------------
TPopupMenu* GetPopupMenu(TStringGrid *grd)
{

    TPopupMenu* pm = grd->PopupMenu;
    if(pm==NULL)
    {
        pm = new TPopupMenu( grd->Owner );
        grd->PopupMenu = pm;
    }
    return pm;
}
//---------------------------------------------------------------------------
void AddCopyItemToPopupMenu(TStringGrid *grd)
{
    static PMCopyHlpr hndl_;
    TPopupMenu *pm = GetPopupMenu(grd);

    TMenuItem *menu = new TMenuItem(grd);
    //menu->Caption = "-";
    //pm->Items->Add(menu);

    //menu = new TMenuItem(grd);
    menu->Caption = "Копировать";
    MenuToGrdMap::Instance().menuToGrd_[menu] = grd;
    pm->Items->Add(menu);
    menu->OnClick =  hndl_.OnClick;
    menu->ShortCut = ShortCut( Word('C'), TShiftState() << ssCtrl );
}
void AddPastItemToPopupMenu(TStringGrid *grd)
{
    static PMPastHlpr hndl_;
    TPopupMenu *pm = GetPopupMenu(grd);

    TMenuItem *menu = new TMenuItem(grd);
    //menu->Caption = "-";
    //pm->Items->Add(menu);

    //menu = new TMenuItem(grd);
    menu->Caption = "Вставить";
    MenuToGrdMap::Instance().menuToGrd_[menu] = grd;
    pm->Items->Add(menu);
    menu->OnClick =  hndl_.OnClick;
    menu->ShortCut = ShortCut( Word('V'), TShiftState() << ssCtrl );
}
//---------------------------------------------------------------------------
unsigned GetRowsHeight(TStringGrid *grd)
{
    int ht = 0;
    for( int i=0; i<grd->RowCount; ++i )
        ht += grd->RowHeights[i];
    return ht;
}

std::map< std::pair<TStringGrid*,int>, bool > selectedRows_;

bool IsRowSelected( TStringGrid *grd, int row )
{

    return !selectedRows_[std::make_pair(grd,row)];
}

void SelectRow( TStringGrid *grd, int row, bool v )
{
    selectedRows_[std::make_pair(grd,row)] = !v;
    RedrawRow(grd, row);
}

void InveertRowselection( TStringGrid *grd, int row )
{
	SelectRow( grd, row, !IsRowSelected( grd, row ) );
}

void InvertRowSelectionOfSelection(TStringGrid *grd)
{
	const TGridRect sel = grd->Selection;
    bool oneIsSelected = 0;
    for( int ro=sel.Top; ro<=sel.Bottom; ++ro )
    	oneIsSelected |= IsRowSelected( grd, ro );
    for( int ro=sel.Top; ro<=sel.Bottom; ++ro )
    	SelectRow( grd, ro, !oneIsSelected );
}


AnsiString RowsSelectionInfoToStr(TStringGrid *grd)
{
	const int rc = grd->RowCount;
	if( rc==0 ) return "";
    const unsigned sz = rc - 1;
	std::vector<int> rsNfo( sz );
    for( unsigned i=0; i<sz; ++i )
    	rsNfo[i] = IsRowSelected( grd, i+1 );
    return my::FormatBitset( rsNfo ).c_str();

}

void SetRowsSelectionInfo(TStringGrid *grd, const AnsiString& strRsNfo)
{
	const int rc = grd->RowCount;
	if( rc==0 ) return;
	std::vector<int> rsNfo;
    my::CreateBitset( strRsNfo.c_str(), rsNfo );
    rsNfo.resize(rc-1,false);
    const int sz = rsNfo.size();
    for( int i=0; i<rc-1; ++i )
    	if( i<sz && rsNfo[i]==false )
 	   		SelectRow( grd, i+1, false );
}

AnsiString ColWidthsToStr(TStringGrid *grd)
{
	AnsiString ret;
    for( int cl=0; cl<grd->ColCount; ++cl )
    	ret = ret + IntToStr( grd->ColWidths[cl] )+" ";
    return ret;
}

void SetColWidthsAsStr(TStringGrid *grd, const AnsiString& strColWidth)
{
	std::stringstream sstr( strColWidth.c_str() );

    int width = 0;
    sstr >> width;
    for( int col = 0; !sstr.fail() && col<grd->ColCount; sstr >> width, ++col )
    	grd->ColWidths[col] = width;
}

void SetRowsHeightsAsStr(TStringGrid *grd, const AnsiString& str)
{
	std::stringstream sstr( str.c_str() );

    int h = 0;
    sstr >> h;
    for( int row = 0; !sstr.fail() && row<grd->RowCount; sstr >> h, ++row )
    	grd->RowHeights[row] = h;
}

int GetActualRowsTotalHeight(TStringGrid *grd)
{
    int ret = 0;
    for( int row=0; row<grd->RowCount; ++row )
        ret += grd->RowHeights[row];
    return ret;
}




}; // namespace Grd
}; // namespace my
