//---------------------------------------------------------------------------
#ifndef SGrdHlprH
#define SGrdHlprH
//---------------------------------------------------------------------------
#include "boost\shared_ptr.hpp"
#include "boost\noncopyable.hpp"

//---------------------------------------------------------------------------
#include <Grids.hpp>

//my
#include "myformat.hpp"
//---------------------------------------------------------------------------
//#include "boost/tuple/tuple.hpp"
//#include "boost\tuple\tuple_comparison.hpp"
//---------------------------------------------------------------------------
namespace my
{
	namespace GridLineT
	{
		enum{ top, left };
	};
	//using boost::tuples::tuple;
	//using boost::tuples::make_tuple;
	//using boost::tuples::tie;
	//---------------------------------------------------------------------------
	//       ”точнени€ типов дл€ работы с €чейками таблиц TStringGrid
	//---------------------------------------------------------------------------
	//  €чейка таблицы
    namespace Grd
    {
    	typedef std::pair< TStringGrid*, std::pair<int,int> > GridCell;

        GridCell GridCellT( TStringGrid* grd, int col, int row )
        {
        	return GridCell( grd, std::make_pair(col,row) );
        }

        typedef std::pair< TStringGrid*, int > GridCol;
        typedef  GridCol GridRow;
        
        void SetTxt(GridCell& cell, const AnsiString& s);
        AnsiString GetTxt(const GridCell& cell);

        // ќтрисовать €чейку col,row в таблице g
		void RedrawCell(GridCell cell);
        void RedrawCell(TStringGrid* grd, int col, int row);

        void RedrawRow(TStringGrid* grd, int row);
        void RedrawCol(TStringGrid* grd, int row);
        void RedrawGrid(TStringGrid* grd);


        // ѕопадает ли €чейка col,row в фиксированную область таблицы
		bool IsCellFixed(const GridCell& cell);
        bool IsCellInEditMode( const GridCell& cell );
        void SetCellIfNotEdited(GridCell cell, AnsiString val);

        // ќтрисовать пр€моугольник left, top, right, bottom в таблице g
		void RedrawRect(TStringGrid* g, TRect rect);

        // ¬озвращает видимый пр€моугольник нефиксированных €чеек таблицы
        TRect VisRect( TStringGrid* g);

        // ¬џбран ли диапазон
        bool IsRangeSelected( const TStringGrid* g );


	//---------------------------------------------------------------------------

	// ¬озвращает видимый пр€моугольник нефиксированных €чеек таблицы
	TRect VisRect(TStringGrid* g);

	// сделать таблицу редактируемой или нет
	void SetEditing(TStringGrid* grd, bool fl);

	// заполнена ли строка
	bool FilledRow( TStringGrid* grd, int row);

	// €чейка под курсором
	TPoint HotCell(TStringGrid* grd);

	void ClearTStringGrid( TStringGrid* grd, int cl0 = 0, int ro0 = 0 );

    // получить форматированный видимый текст
    AnsiString GetFormatedVisibleText( const TStringGrid* grd,
    	unsigned col_from, unsigned col_to,
        unsigned row_from, unsigned row_to);

    void SetColVis(const TStringGrid* grd, unsigned col, bool vis);
    void SetRowVis(const TStringGrid* grd, unsigned row, bool vis);

    int GetLastVisCol(TStringGrid* grd)
    {
    	int cl=grd->ColCount-1;
    	for( ; cl>-1; --cl ) if( grd->ColWidths[cl]>0 ) break;
        return cl;
    }

    int GetLastVisRow(TStringGrid* grd)
    {
    	int ro=grd->RowCount-1;
    	for( ; ro>-1; --ro ) if( grd->RowHeights[ro]>0 ) break;
        return ro;
    }

    AnsiString GetVisColsInfo(TStringGrid* grd);
    void SetVisColsInfo(TStringGrid* grd, const AnsiString&);

    // вызвать диалог дл€ настройуи видимых столбцов
    void SetupVisibleCols(TStringGrid *grd);
    void SetupVisibleColsOnMouseDownRow0(TObject *grd, TMouseButton Button, TShiftState Shift, int X, int Y, unsigned titleNum);
    void SetupVisibleRowsOnMouseDownCol0(TObject *grd, TMouseButton Button, TShiftState Shift, int X, int Y, unsigned titleNum);

    void SetupVisibleRows(TStringGrid* grd, unsigned titleNum);

    AnsiString GetVisRowsInfo(TStringGrid* grd);
    void SetVisRowsInfo(TStringGrid* grd, const AnsiString&);

    void ClearSelection(const TStringGrid* grd);

    // копировать в буфер
    void CopyCellsToClipboard( const TStringGrid* grd );
    void PastCellsFromClipboard(const TStringGrid* grd);

    template<typename T>
    void SetColumn( TStringGrid* grd, const int cl, T str, T strEnd, int row0 = 0)
    {
    	for( int ro=row0; ro<grd->RowCount && str!=strEnd ; ++ro )
        	grd->Cells[cl][ro] = *str++;
    }


    template<typename T>
    void SetRow( TStringGrid* grd, const int ro, T str, T strEnd, int col0 = 0)
    {
    	for( int cl=col0; cl<grd->ColCount && str!=strEnd ; ++cl )
        	grd->Cells[cl][ro] = *(str++);
    }

    template<typename T>
    void SetCol( TStringGrid* grd, const int cl, T str, T strEnd, int row0 = 0)
    {
    	for( int ro=row0; ro<grd->RowCount && str!=strEnd ; ++ro )
        	grd->Cells[cl][ro] = *(str++);
    }

    void TieWithColsVisiblePopup( TStringGrid* grd, TNotifyEvent onPopup = NULL );
    void TieWithRowsVisiblePopup( TStringGrid* grd, TNotifyEvent onPopup = NULL  );

    void DeleteRow(TStringGrid* grd, int row, int count);
    void DeleteCol(TStringGrid* grd, int col, int count);

    void SetCellByTtl(TStringGrid *grd, const AnsiString& ttl, int row, const AnsiString& s);
	AnsiString GetCellByTtl(TStringGrid *grd, const AnsiString& ttl, int row);

    TPopupMenu* GetPopupMenu(TStringGrid *grd);

    void AddCopyItemToPopupMenu(TStringGrid *grd);
    void AddPastItemToPopupMenu(TStringGrid *grd);

    void AddSkipPastCol(TStringGrid* grd, int col);
    void AddSkipPastRow(TStringGrid* grd, int row);

    unsigned GetRowsHeight(TStringGrid *grd);


    const TColor unselectedRowColor = 0xDDDDDD;
    bool IsRowSelected( TStringGrid *grd, int row );

	void SelectRow( TStringGrid *grd, int ro, bool v );
	void InveertRowselection( TStringGrid *grd, int row );
	void InvertRowSelectionOfSelection(TStringGrid *grd);

    AnsiString  RowsSelectionInfoToStr(TStringGrid *grd);
    void SetRowsSelectionInfo(TStringGrid *grd, const AnsiString& strRsNfo);

    AnsiString  ColWidthsToStr(TStringGrid *grd);
    void SetColWidthsAsStr(TStringGrid *grd, const AnsiString& strColWidth);
    void SetRowsHeightsAsStr(TStringGrid *grd, const AnsiString& str);

    bool IsCellSelected( TStringGrid* grd, int col, int row )
    {
    	const TGridRect r = grd->Selection;
        return col>=r.Left && col<=r.Right && row>=r.Top && row<=r.Bottom;
    }

    int GetActualRowsTotalHeight(TStringGrid *grd);



    }; // namespace Grd
}; // namespace my



//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
