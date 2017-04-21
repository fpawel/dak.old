//---------------------------------------------------------------------------


#ifndef uFrameGraphH
#define uFrameGraphH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Chart.hpp>
#include <ExtCtrls.hpp>
#include <Series.hpp>
#include <TeEngine.hpp>
#include <TeeProcs.hpp>
#include <ComCtrls.hpp>

#include <Buttons.hpp>
#include <Menus.hpp>


// boost
#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"
#include <CheckLst.hpp>
#include <Dialogs.hpp>

class CrossHairHelper;
//---------------------------------------------------------------------------
#include "free_types_.h"


class DateTimeFormater : public boost::noncopyable
{
public:
	virtual AnsiString DateTimeToString(TDateTime) const= 0;
    virtual bool TryStringToDateTime( const AnsiString&, TDateTime&  )  const = 0;
    virtual ~DateTimeFormater() {}
};

class DefaultTimeFormater : public DateTimeFormater
{
public:
	virtual AnsiString DateTimeToString(TDateTime tt)  const
    {
    	return DateTimeToStr( tt );
    }

    virtual bool TryStringToDateTime( const AnsiString& s, TDateTime& tt )  const
    {
    	return TryStrToDateTime(s, tt);
    }
    virtual ~DefaultTimeFormater() {}
};

class TFrameDropDownPanel;

class TFrameGraph : public TFrame
{
__published:	// IDE-managed Components
	TScrollBox *ScrollBox1;
	TChart *chrt_;
    TSplitter *Splitter1;
	TPanel *grpBxY;
	TLabel *Label5;
	TLabel *Label6;
	TLabel *shkalaY;
	TEdit *edYMin;
	TCheckBox *cbAutoYMin;
	TEdit *edYMax;
	TCheckBox *cbAutoYMax;
	TPanel *grpBxX;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label1;
	TLabel *shkalaX;
	TEdit *edXMin;
	TCheckBox *cbAutoXMin;
	TEdit *edXMax;
	TCheckBox *cbAutoXMax;
	TEdit *edDateTimeFormat;
	TPopupMenu *popupMenuGraph;
	TMenuItem *N1;
	TMenuItem *N2;
	TMenuItem *menuSeries;
	TMenuItem *N5;
	TMenuItem *N6;
	TMenuItem *N7;
	TMenuItem *N8;
	TMenuItem *N9;
	TColorDialog *ColorDialog1;
	TListView *lvSeriesList;
	TPanel *panelGraphView;
	TCheckBox *cbAutoLimitY;
	TCheckBox *cbNoZoom;
	TPanel *panelFile;
	TLabel *btnNewFile;
	TLabel *btnClearVisiblePoints;
	TLabel *btnOpen;
	TLabel *btnSave;
	TLabel *btnBrushColor;
	TCheckBox *cbShowPoints;
	TCheckBox *cbCrossHair;
	TCheckBox *cbShowLegend;
	TOpenDialog *OpenDialog1;
	TSaveDialog *SaveDialog1;
	void __fastcall cbAutoXMinClick(TObject *Sender);
	void __fastcall cbAutoXMaxClick(TObject *Sender);
	void __fastcall cbAutoYMinClick(TObject *Sender);
	void __fastcall cbAutoYMaxClick(TObject *Sender);
	void __fastcall edXMinChange(TObject *Sender);
	void __fastcall edXMaxChange(TObject *Sender);
	void __fastcall edYMinChange(TObject *Sender);
	void __fastcall edYMaxChange(TObject *Sender);
	void __fastcall chrt_MouseMove(TObject *Sender, TShiftState Shift, int X,
          int Y);
	void __fastcall cbCrossHairClick(TObject *Sender);
	void __fastcall chrt_AfterDraw(TObject *Sender);
	void __fastcall chrt_MouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall chrt_MouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall cbAutoLimitYClick(TObject *Sender);
	void __fastcall edXMinKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall chrt_Zoom(TObject *Sender);
	void __fastcall btnUndoZoomClick(TObject *Sender);
	void __fastcall cbShowPointsClick(TObject *Sender);
	void __fastcall edDateTimeFormatChange(TObject *Sender);
	void __fastcall cbNoZoomClick(TObject *Sender);
	void __fastcall btnClearVisiblePointsClick(TObject *Sender);
	void __fastcall btnNewFileClick(TObject *Sender);
	void __fastcall cbSeriesListClick(TObject *Sender);
	void __fastcall cbShowLegendClick(TObject *Sender);
	void __fastcall N1Click(TObject *Sender);
	void __fastcall N2Click(TObject *Sender);
	void __fastcall N5Click(TObject *Sender);
	void __fastcall N7Click(TObject *Sender);
	void __fastcall lvSeriesListCustomDrawItem(TCustomListView *Sender,
          TListItem *Item, TCustomDrawState State, bool &DefaultDraw);
	void __fastcall lvSeriesListCustomDrawSubItem(TCustomListView *Sender,
          TListItem *Item, int SubItem, TCustomDrawState State,
          bool &DefaultDraw);
	void __fastcall popupMenuGraphPopup(TObject *Sender);
	void __fastcall lvSeriesListMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall btnBrushColorClick(TObject *Sender);
	void __fastcall btnOpenClick(TObject *Sender);
	void __fastcall btnSaveClick(TObject *Sender);
private:	// User declarations
	TForm *form_;
    const DateTimeFormater* dateTimeFormater_;

    VDouble xMarks_, x0Marks_, x1Marks_;
    std::vector<AnsiString> names_;

	// для отрисовки курсора графика
    boost::shared_ptr<CrossHairHelper> drawCross_;


    TChartAxis *xAx_, *yAx_;

    double xMin_, yMin_, xMax_, yMax_;

    TFrameDropDownPanel *pnX, *pnY, *pnGraphTools, *pnSeriesList, *pnFile;

    AnsiString fileName_;

    void SetFile( const AnsiString &fn );
    void OpenFile(const AnsiString&);

    void UpdateCurrentGraphInfo();
    void SetAutomaticYOrders10Precent();
    void ShowChartNfo();

    void OnChartMouseOcurence( bool isEntered );

    void __fastcall grph_AfterDrawValues(TObject *sndr);
    void __fastcall grph_BeforDrawValues(TObject *sndr);

    void __fastcall OnWMDropeFiles(TMessage Message);

    BEGIN_MESSAGE_MAP
    	VCL_MESSAGE_HANDLER(WM_DROPFILES, TMessage, OnWMDropeFiles);
	END_MESSAGE_MAP(TWinControl);
public:		// User declarations


	__fastcall TFrameGraph(TComponent* Owner, TWinControl* parent,
    	const DateTimeFormater* dateTimeFormater = NULL);
    __fastcall ~TFrameGraph();

    TChartSeries* AddSeries(TChartSeries *ser = NULL);
    
    void RemoveSeries(unsigned idx);
    void RemoveSeries(TChartSeries *ser);

    TChartSeries* GetSeries(unsigned) const;
    TChartSeries* ActiveGraph();
    bool IsValidSeries(TChartSeries*);

   	double XMin() const { return xMin_; }
    double YMin() const { return yMin_; }
    double XMax() const { return xMax_; }
    double YMax() const { return yMax_; }

    void OnMouseWheel( TWinControl* ctrl, bool isUp );

    void __fastcall FormMouseWheelUp(TObject *Sender,
      TShiftState Shift, const TPoint &mousePos, bool &Handled);

    void __fastcall FormMouseWheelDown(TObject *Sender,
      TShiftState Shift, const TPoint &mousePos, bool &Handled);

    void SetXMarks( const VDouble& marks, const VDouble &x0, const VDouble &x1,
    	const std::vector<AnsiString>& names );

    int SeriesCount() const;

    void UndoZoom();

    void SetActiveSeries(int);
    void SetActiveSeries(TChartSeries*);
    int GetActiveSeries();

    void (__closure *OnSetActiveSeries )(TChartSeries*, int);
    void (__closure *OnAddSeries )(TChartSeries*);
    void (__closure *OnDeletSeries )(TChartSeries*);
    void (__closure *OnBeforClearSeriesList )();
};
//---------------------------------------------------------------------------
extern PACKAGE TFrameGraph *FrameGraph;
//---------------------------------------------------------------------------
#endif
