//---------------------------------------------------------------------------
#include <vcl.h>
//std
#include <math>
#include <fstream>
#pragma hdrstop

#include "uFrameGraph.h"
#include "uChartHelper.h"
#include "AnsiStringUtils_.h"
#include "MyVclUtils_.h"

#include <Math.hpp>
#include <DateUtils.hpp>
#include <Inifiles.hpp>
#include "uFrameDropDownPanel.h"

#include "MyModFN.hpp"
#include "MyFileCtrl.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//TFrameGraph *FrameGraph;

TIniFile* GetINI() {
    static TIniFile* ret = NULL;
    if(ret==NULL)
    	ret = new TIniFile( AnsiString(MyGetExePath().c_str())+"charts.ini" );
    return ret;
}

void SetCheckedBox(TCheckBox* cb, bool checked)
{
	TNotifyEvent onClick = cb->OnClick;
    cb->OnClick = NULL;
    cb->Checked = checked;
    cb->OnClick = onClick;
}


void InitTrackBar(TTrackBar* tb)
{
	tb->Max = 1000000;
	tb->Min = 0;
    tb->Position = 0;
}

void SetTrackBarPos(TTrackBar* tb, int position)
{
    TFrame* frame = dynamic_cast<TFrame*>(tb->Owner);
    assert(frame);
    TForm* form =  dynamic_cast<TForm*>(frame->Owner);
    assert(frame);
	if( form->ActiveControl==tb ) return;
	TNotifyEvent onClick = tb->OnChange;
    tb->OnChange = NULL;
   	tb->Position = position>=tb->Max-10 ? tb->Max-10 : position;
    tb->OnChange = onClick;
}

void SetTrackBarPosAsVal(TTrackBar* tb, double len, double val)
{
	//const double trackVal = ( (ordrs.maxX-ordrs.minX)*tbXMin->Position)/tbXMin->Max + ordrs.minX;
	const int tbLen = tb->Max - tb->Min, offSet = Ceil( tbLen * 0.1 ),
    posMin = tb->Min + offSet, posMax = tb->Max - offSet;
    SetTrackBarPos(tb, posMin + Ceil( (posMax-posMin)*( val )/len ) );
}
//------------------------------------------------------------------------------
double GetTrackBarPosAsPercent(TTrackBar* tb)
{
	//const double trackVal = ( (ordrs.maxX-ordrs.minX)*tbXMin->Position)/tbXMin->Max + ordrs.minX;
	const int tbLen = tb->Max - tb->Min, offSet = Ceil( tbLen * 0.1 ),
    posMin = tb->Min + offSet, posMax = tb->Max - offSet,
    dPos = tb->Position - posMin, lenPos = posMax - posMin;
    return dPos*1.0/(lenPos*1.0);
}
//------------------------------------------------------------------------------
void SetTEditText( TEdit* ed, const AnsiString& s )
{
    TNotifyEvent onClick = ed->OnChange;
    ed->OnChange = NULL;
    const int selStr = ed->SelStart, selLen = ed->SelLength;
   	ed->Text = s;
    ed->SelStart = selStr;
    ed->SelLength = selLen;
    ed->OnChange = onClick;
}
//------------------------------------------------------------------------------
void MakeUpDownCircled(TObject* sndr)
{
	TUpDown* ctrl =  dynamic_cast<TUpDown*>(sndr);
    if( ctrl->Position == ctrl->Min )
    	ctrl->Position = ctrl->Max;
}
//---------------------------------------------------------------------------

const DateTimeFormater* CreateDateTimeFormater( const DateTimeFormater* p )
{
	if( p==NULL )
    	return new DefaultTimeFormater();
    else
        return p;
}


//---------------------------------------------------------------------------
__fastcall TFrameGraph::TFrameGraph(TComponent* Owner, TWinControl* parent,
	const DateTimeFormater* dateTimeFormater )
	: TFrame(Owner),
    form_( static_cast<TForm*>(Owner) ),
    drawCross_ ( new CrossHairHelper(chrt_) ),
    dateTimeFormater_( CreateDateTimeFormater( dateTimeFormater) ),
    xAx_(chrt_->BottomAxis), yAx_(chrt_->LeftAxis),
    OnSetActiveSeries(NULL),
    OnAddSeries(NULL),
    OnDeletSeries(NULL),
    OnBeforClearSeriesList(NULL)
{
	static unsigned count = 0;
    Name = "FrameGraph"+IntToStr(count++);

	Parent = parent;
    Align = alClient;

	drawCross_->OnMouseOcurence = OnChartMouseOcurence;

    MakeLabelAsButton( btnBrushColor );
    MakeLabelAsButton( btnSave );
    MakeLabelAsButton( btnNewFile );
    MakeLabelAsButton( btnOpen );
    MakeLabelAsButton( btnClearVisiblePoints );

    //TFrameDropDownPanel*
    pnFile = new TFrameDropDownPanel(this, ScrollBox1, "График");
    pnFile->AddAllClienPanel(panelFile);

    pnX = new TFrameDropDownPanel(this, ScrollBox1, "Ось X (время)");
    pnX->AddAllClienPanel(grpBxX );
    pnY = new TFrameDropDownPanel(this, ScrollBox1, "Ось Y");
    pnY->AddAllClienPanel(grpBxY );

    pnSeriesList = new TFrameDropDownPanel(this, ScrollBox1, "Серии");
    pnSeriesList->AddAllClienPanel(lvSeriesList);

    pnGraphTools = new TFrameDropDownPanel(this, ScrollBox1, "Вид");
    pnGraphTools->AddAllClienPanel(panelGraphView);    

    pnX->Expand();
    pnX->Colapse();

    lvSeriesList->Items->Clear();

    const AnsiString colorsFN = AnsiString(MyGetExePath().c_str()) + "graph.colors";
    if( MyFileExists(colorsFN) )
    	ColorDialog1->CustomColors->LoadFromFile( colorsFN );

    TIniFile *ini = GetINI();
    chrt_->BackColor = ini->ReadInteger( "colors", "фон", chrt_->BackColor );
    lvSeriesList->Color = chrt_->BackColor;
    chrt_->Color = ini->ReadInteger( "colors", "шкала", chrt_->Color );

    ::DragAcceptFiles( Handle, TRUE );
}
//---------------------------------------------------------------------------
__fastcall TFrameGraph::~TFrameGraph()
{
	ColorDialog1->CustomColors->SaveToFile( AnsiString(MyGetExePath().c_str()) + "graph.colors" );
    TIniFile *ini = GetINI();
    ini->WriteInteger( "colors", "шкала", chrt_->Color );
    for( int idx=0; idx<chrt_->SeriesCount(); ++idx )
    {
    	TChartSeries *ser = chrt_->Series[idx];
        ini->WriteInteger( "colors", idx, ser->SeriesColor );
    }
}
//---------------------------------------------------------------------------
void TFrameGraph::SetFile( const AnsiString &fn )
{
   fileName_ = fn;
   chrt_->Title->Visible = !fn.IsEmpty();
   chrt_->Title->Text->Text = "Файл \""+fn+"\"";
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::OnWMDropeFiles(TMessage Msg)
{
	if (Msg.Msg != WM_DROPFILES) return;
    HDROP hDrop = (HDROP) Msg.WParam;
    const UINT filesCount = ::DragQueryFile( hDrop, 0xFFFFFFFF, NULL, 0 );
    if( filesCount==0 ) return;

    enum { iFile = 0 };

    const UINT filesStringLen = ::DragQueryFile( hDrop, iFile, NULL, 0 );
    std::vector<char> filesStringVec( filesStringLen+2, '\0' );
    LPTSTR lpszFile = &filesStringVec.at(0);
    ::DragQueryFile( hDrop, iFile, lpszFile, filesStringLen+1 );
    const AnsiString fn = lpszFile;
    ::DragFinish( hDrop);
    OpenFile(fn);
}
//---------------------------------------------------------------------------
void TFrameGraph::OpenFile(const AnsiString& fn)
{
	if( OnBeforClearSeriesList )
    	OnBeforClearSeriesList();

	for( int idx=0; idx<chrt_->SeriesCount(); ++idx )
    {
    	TChartSeries *ser = chrt_->Series[idx];
        if( OnDeletSeries ) OnDeletSeries(ser);
    }

	if( MyLoadTChartFromFile( fn, chrt_ ) )
    	SetFile( fn );

    lvSeriesList->Items->Clear();
    for( int idx=0; idx<chrt_->SeriesCount(); ++idx )
    {
    	TChartSeries *ser = chrt_->Series[idx];
    	ser->AfterDrawValues = grph_AfterDrawValues;
        TListItem *listItem = lvSeriesList->Items->Add();
    	listItem->Caption = ser->Title;
        listItem->Checked = true;
        if( OnAddSeries ) OnAddSeries(ser);
    }
    pnSeriesList->SetExpandedHeight( lvSeriesList->Items->Count*21 + 50 );

	
}
//---------------------------------------------------------------------------
TChartSeries* TFrameGraph::ActiveGraph()
{
    const int
    	cnt = SeriesCount(),
        cbIdx = lvSeriesList->ItemIndex;
    if(cnt==0) return NULL;
    if( cbIdx>-1 && cbIdx<cnt && chrt_->Series[cbIdx]->Active )
    	return chrt_->Series[cbIdx];
    return static_cast<TChartSeries*>( GetFirstActiveChartSeries(chrt_) );
}
//---------------------------------------------------------------------------
TChartSeries* TFrameGraph::AddSeries(TChartSeries *ser)
{
	if( ser==NULL )ser =  new TFastLineSeries(this);
    ser->AfterDrawValues = grph_AfterDrawValues;

    ser->XValues->DateTime = true;
    ser->ParentChart = chrt_;
    const int idx = chrt_->SeriesCount()-1;

    TIniFile *ini = GetINI();
    ser->SeriesColor = ini->ReadInteger( "colors", idx, ser->SeriesColor );

    TListItem *listItem = lvSeriesList->Items->Add();
    listItem->Caption = ser->Title;

    pnSeriesList->SetExpandedHeight( lvSeriesList->Items->Count*21 + 50 );
    listItem->Checked = true;
    if( OnAddSeries ) OnAddSeries(ser);

    return ser;
}
//---------------------------------------------------------------------------
void TFrameGraph::RemoveSeries(unsigned idx)
{
	assert( idx<(unsigned)chrt_->SeriesCount() );
    lvSeriesList->Items->Delete(idx);
    TChartSeries* ser = chrt_->Series[idx];
    ser->ParentChart = NULL;
    pnSeriesList->SetExpandedHeight( lvSeriesList->Items->Count*21 + 50 );
}
void TFrameGraph::RemoveSeries(TChartSeries *ser)
{
	const int sz = chrt_->SeriesCount();
    for( int idx=0; idx<sz; ++idx )
    	if( chrt_->Series[idx]==ser )
        {
        	RemoveSeries(idx);
            return;
        }
}
//---------------------------------------------------------------------------
int TFrameGraph::SeriesCount() const
{
    return chrt_->SeriesCount();
}
//---------------------------------------------------------------------------
void TFrameGraph::SetActiveSeries(int n)
{
    const int sz = chrt_->SeriesCount();
    n = (n>-1 && n<sz) ? n : -1;
    TChartSeries *ser = (n>-1 && n<sz) ? chrt_->Series[n] : NULL;
    for( int i=0; i<sz; ++i ) chrt_->Series[i]->Active = (i==n);
    if( OnSetActiveSeries ) OnSetActiveSeries(ser, n);
}
//---------------------------------------------------------------------------
void TFrameGraph::SetActiveSeries(TChartSeries* ser)
{
	const int sz = chrt_->SeriesCount();
    //int n = -1;
    for( int i=0; i<sz; ++i )
    {
    	chrt_->Series[i]->Active = (chrt_->Series[i]==ser);
        if( chrt_->Series[i]->Active && OnSetActiveSeries )
        	OnSetActiveSeries(ser, i);
    }
}
//---------------------------------------------------------------------------
int TFrameGraph::GetActiveSeries()
{
    return chrt_->SeriesList->IndexOf( ActiveGraph() );
}
//---------------------------------------------------------------------------
bool TFrameGraph::IsValidSeries(TChartSeries* ser)
{
	const int sz = chrt_->SeriesCount();
    for( int i=0; i<sz; ++i )
        if( chrt_->Series[i]==ser )
        	return true;
    return false;

}
//---------------------------------------------------------------------------
void TFrameGraph::UndoZoom()
{
	cbAutoLimitY->Checked = 0;
	chrt_->UndoZoom();
    xAx_->Automatic = 1;
    yAx_->Automatic = 1;
}
//---------------------------------------------------------------------------
TChartSeries* TFrameGraph::GetSeries(unsigned n) const
{
	return chrt_->Series[n];
}
//---------------------------------------------------------------------------
void TFrameGraph::OnChartMouseOcurence( bool isEntered )
{
	pnY->Label5->Caption = "Ось Y";
    pnX->Label5->Caption = "Ось X (время)";
}
//---------------------------------------------------------------------------
void TFrameGraph::OnMouseWheel( TWinControl* ctrl, bool isUp )
{
	if( ctrl==edXMin ) IncAxOrder( xAx_, ax_Min, isUp ); else
    if( ctrl==edXMax ) IncAxOrder( xAx_, ax_Max, isUp ); else
    if( ctrl==edYMin ) IncAxOrder( yAx_, ax_Min, isUp ); else
    if( ctrl==edYMax ) IncAxOrder( yAx_, ax_Max, isUp );
}
//---------------------------------------------------------------------------
void TFrameGraph::SetAutomaticYOrders10Precent()
{
	TChartSeries* ser = ActiveGraph();
    if( ser==0 || !cbAutoLimitY->Checked ) return;
    const std::pair<double,double> yLims = GetYLimitsOfSeries(ser);
    // границы по оси Y
    double YAxisMin = chrt_->LeftAxis->Minimum, YAxisMax = chrt_->LeftAxis->Maximum;
    yAx_->CalcMinMax(YAxisMin, YAxisMax);
    const double yLimOffset = (yLims.second - yLims.first)*0.05,
    	yMin = yLims.first - yLimOffset, yMax = yLims.second + yLimOffset;
    	if( ( std::fabs(YAxisMin-yMin) >yLimOffset || fabs(YAxisMax-yMax) >yLimOffset )&&
        	(yMin-yLimOffset<yMax+yLimOffset) )
        	yAx_->SetMinMax(yMin-yLimOffset, yMax+yLimOffset);
}
//---------------------------------------------------------------------------
void TFrameGraph::ShowChartNfo()
{

    // границы по оси Y X
    yMin_=yAx_->Minimum;
    yMax_=yAx_->Maximum;
    xMin_=xAx_->Minimum;
    xMax_=xAx_->Maximum;

   	SetTEditText( edXMin, dateTimeFormater_->DateTimeToString(xMin_ ) );
    SetTEditText( edXMax, dateTimeFormater_->DateTimeToString(xMax_ ) );
    shkalaX->Caption = "Шкала: "+dateTimeFormater_->DateTimeToString( std::fabs(xMax_-xMin_) );

    SetTEditText( edYMin, FormatFloat( yAx_->AxisValuesFormat, yMin_ ) );
    SetTEditText( edYMax, FormatFloat( yAx_->AxisValuesFormat, yMax_ ) );

    const double
    	absScale = std::fabs(yMax_-yMin_),
        vScale = yMax_==0 ? 0 : std::fabs(absScale*100.0/yMax_);
    const AnsiString strVScale = yMax_==0 || absScale==0 ? AnsiString(" ") : FormatFloat( "#0.#", vScale)+"%";

    shkalaY->Caption =AnsiString().sprintf("Шкала: %s %s",
        FormatFloat( yAx_->AxisValuesFormat, absScale), strVScale );

    cbAutoXMin->Checked = xAx_->AutomaticMinimum;
    cbAutoXMax->Checked = xAx_->AutomaticMaximum;
    cbAutoYMin->Checked = yAx_->AutomaticMinimum;
    cbAutoYMax->Checked = yAx_->AutomaticMaximum;

    SetTEditText( edDateTimeFormat, xAx_->DateTimeFormat );

    cbNoZoom->OnClick = NULL;
    cbNoZoom->Checked = cbAutoXMin->Checked && cbAutoXMax->Checked && cbAutoYMin->Checked && cbAutoYMax->Checked;
    cbNoZoom->OnClick = cbNoZoomClick;

    cbShowLegend->OnClick = NULL;
    cbShowLegend->Checked = chrt_->Legend->Visible;
    cbShowLegend->OnClick = cbShowLegendClick;
}
//---------------------------------------------------------------------------
void TFrameGraph::UpdateCurrentGraphInfo()
{
	SetAutomaticYOrders10Precent();
    ShowChartNfo();
}
//---------------------------------------------------------------------------
void TFrameGraph::SetXMarks(const VDouble& marks, const VDouble &x0,
	const VDouble &x1, const std::vector<AnsiString>& names)
{
    xMarks_ = marks;
    x0Marks_ = x0;
    x1Marks_ = x1;
    names_ = names;
    TChartSeries* ser = ActiveGraph();
    if( ser ) ser->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::cbAutoXMinClick(TObject *Sender)
{
	xAx_->AutomaticMinimum = cbAutoXMin->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::cbAutoXMaxClick(TObject *Sender)
{
	xAx_->AutomaticMaximum = cbAutoXMax->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::cbAutoYMinClick(TObject *Sender)
{
	yAx_->AutomaticMinimum = cbAutoYMin->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::cbAutoYMaxClick(TObject *Sender)
{
	yAx_->AutomaticMaximum = cbAutoYMax->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::edXMinChange(TObject *Sender)
{
    TDateTime tm = 0;
    if( dateTimeFormater_->TryStringToDateTime(edXMin->Text,tm)  )
        SetAxisMin( xAx_, tm.Val );
}
//---------------------------------------------------------------------------

void __fastcall TFrameGraph::edXMaxChange(TObject *Sender)
{
	TDateTime tm = 0;
    if( dateTimeFormater_->TryStringToDateTime(edXMax->Text,tm) )
        SetAxisMax( xAx_, tm.Val );
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::edYMinChange(TObject *Sender)
{
    double val;
    if( !MyTryStrToFloat( edYMin->Text, &val)  ) return;
    SetAxisMin( yAx_, val );
    cbAutoLimitY->Checked  = false;
}
//---------------------------------------------------------------------------

void __fastcall TFrameGraph::edYMaxChange(TObject *Sender)
{
    double val;
    if( !MyTryStrToFloat( edYMax->Text, &val)  ) return;
    SetAxisMax( yAx_, val );
    cbAutoLimitY->Checked  = false;
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::chrt_MouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
    TChartSeries* ser = ActiveGraph();
    if( ser==NULL ) return;
    drawCross_->SetColor( ser->SeriesColor );
	if( Shift.Contains(ssLeft)||Shift.Contains(ssRight) )
    	drawCross_->Hide();
    else
		if (cbCrossHair->Checked)
        {
        	drawCross_->SetSeries( ser );
			drawCross_->OnMouseMove(X,Y);
        }

	double x,y;
	ser->GetCursorValues(x,y);
    // получить индекс y по x или -1
	const int yIdx = GetYValueChartSeries(ser, x);
    //const double vX = ser->XScreenToValue(X), vY = ser->YScreenToValue(Y);
    if(yIdx==-1)
    {
    	pnY->Label5->Caption = "Ось Y";
    	pnX->Label5->Caption = "Ось X (время)";
    }
    else
    {

    	pnX->Label5->Caption =
        	AnsiString().sprintf("X=%s", dateTimeFormater_->DateTimeToString(x) );
        pnY->Label5->Caption =
        	AnsiString().sprintf("Y=%s",FormatFloat(
    	yAx_->AxisValuesFormat, ser->YValue[yIdx] ) );
    }
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::grph_AfterDrawValues(TObject *sndr)
{
    drawCross_->Reset();
    if( cbShowPoints->Checked )
    {
    	TChartSeries* ser = static_cast<TChartSeries*>(sndr);
        DrawSeriesValuesPoints(ser, ser->SeriesColor, 3);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::grph_BeforDrawValues(TObject *sndr)
{
	//
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::chrt_AfterDraw(TObject *Sender)
{
    UpdateCurrentGraphInfo();
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::cbCrossHairClick(TObject *Sender)
{
    if( !cbCrossHair->Checked ) drawCross_->Hide();
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::chrt_MouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	drawCross_->Hide();
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::chrt_MouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	drawCross_->Hide();
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::FormMouseWheelUp(TObject *Sender,
      TShiftState Shift, const TPoint &mousePos, bool &Handled)
{
	//form_->ActiveControl = NULL;
	OnMouseWheel( GetVCLControlAtPos( this,  mousePos ), true );
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::FormMouseWheelDown(TObject *Sender,
      TShiftState Shift, const TPoint &mousePos, bool &Handled)
{
	OnMouseWheel( GetVCLControlAtPos( this,  mousePos ), false );
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::edXMinKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
	if( Key!=VK_UP && Key!=VK_DOWN ) return;
    const bool isUp = Key==VK_UP;
    Key = 0;
    OnMouseWheel( dynamic_cast<TWinControl*>(Sender), isUp );
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::cbAutoLimitYClick(TObject *Sender)
{
	SetAutomaticYOrders10Precent();
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::chrt_Zoom(TObject *Sender)
{
	drawCross_->Hide();
    chrt_->Repaint();
}
//------------------------------------------------------------------------------
void __fastcall TFrameGraph::btnUndoZoomClick(TObject *Sender)
{
	UndoZoom();
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::cbShowPointsClick(TObject *Sender)
{
	for( int i=0; i<chrt_->SeriesCount(); ++i )
    	chrt_->Series[i]->Repaint();

}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::edDateTimeFormatChange(TObject *Sender)
{
	const AnsiString s = static_cast<TEdit*>(Sender)->Text;
	xAx_->DateTimeFormat = s;
    xAx_->AxisValuesFormat = s;
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::cbNoZoomClick(TObject *Sender)
{
	if( cbNoZoom->Checked )	UndoZoom();
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::btnClearVisiblePointsClick(TObject *Sender)
{
   	if( ::MessageBox( Application->Handle, "Удалить видимые точки графика?", "Подтверждение",
    	MB_OKCANCEL | MB_APPLMODAL | MB_ICONQUESTION )!=IDOK )
        return;

	for( int i=0; i<chrt_->SeriesCount(); ++i )
    {
    	TChartSeries *ser = chrt_->Series[i];
    	if( !ser->Active ) continue;
        for( int idx=0; idx<ser->Count(); ++idx )
        {
        	const double x = ser->XValues->Value[idx], y = ser->YValues->Value[idx];
            if( x>XMin() && x<XMax() && y>YMin() && y<YMax() )
            {
            	ser->Delete(idx);
                --idx;
            }
        }
    }
    UndoZoom();
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::btnNewFileClick(TObject *Sender)
{
	if( ::MessageBox(Application->Handle, "Создать новый файл?", "Подтверждение",
    	MB_OKCANCEL | MB_APPLMODAL | MB_ICONQUESTION )!=IDOK )
        return;
    const unsigned null = 0ul;
    while( chrt_->SeriesCount()>0 )
    	RemoveSeries( null );
    UndoZoom();
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::cbSeriesListClick(TObject *Sender)
{
    drawCross_->SetSeries( ActiveGraph() );
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::cbShowLegendClick(TObject *Sender)
{
	chrt_->Legend->Visible = cbShowLegend->Checked;	
}
//---------------------------------------------------------------------------


void __fastcall TFrameGraph::N1Click(TObject *Sender)
{
	const int idx = menuSeries->Tag;
    if( idx<=-1 || idx>=chrt_->SeriesCount() ) return;
    TChartSeries* ser = chrt_->Series[idx];
    ColorDialog1->Color = ser->SeriesColor;
    if( !ColorDialog1->Execute() ) return;
    ser->SeriesColor = ColorDialog1->Color;
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::N2Click(TObject *Sender)
{
	const int idx = menuSeries->Tag;
    if( idx<=-1 || idx>=chrt_->SeriesCount() ) return;
    TFastLineSeries* ser = dynamic_cast<TFastLineSeries*>( chrt_->Series[idx] );
    if( ser==NULL ) return;
    AnsiString Value = ser->LinePen->Width;
    if( InputQuery( "Толщина линии "+ser->Title, "Задайте толщину линии "+ser->Title, Value) )
    	ser->LinePen->Width = StrToInt(Value);
}
//---------------------------------------------------------------------------


void __fastcall TFrameGraph::N5Click(TObject *Sender)
{
	const int idx = menuSeries->Tag;
    if( idx<=-1 || idx>=chrt_->SeriesCount() ) return;
    TChartSeries* ser = chrt_->Series[idx];
    AnsiString Value = ser->Title;
    if( InputQuery( "Наименование графика", "Задайте наименование графика", Value) )
    {
    	ser->Title = Value;
    	lvSeriesList->Items->Item[idx]->Caption = Value;
    }  
}
//---------------------------------------------------------------------------
void __fastcall TFrameGraph::N7Click(TObject *Sender)
{
	const int idx = menuSeries->Tag;
    if( idx<=-1 || idx>=chrt_->SeriesCount() ) return;
    RemoveSeries(idx);
}
//---------------------------------------------------------------------------

void __fastcall TFrameGraph::lvSeriesListCustomDrawItem(
      TCustomListView *Sender, TListItem *Item, TCustomDrawState State,
      bool &DefaultDraw)
{
	TCanvas *cnv = lvSeriesList->Canvas;
    if( Item->Index<chrt_->SeriesCount() )
    	cnv->Font->Color = chrt_->Series[ Item->Index ]->SeriesColor;
    //const TRect rect = Item->DisplayRect( drBounds );    const int h = rect.Height();
}
//---------------------------------------------------------------------------

void __fastcall TFrameGraph::lvSeriesListCustomDrawSubItem(
      TCustomListView *Sender, TListItem *Item, int SubItem,
      TCustomDrawState State, bool &DefaultDraw)
{
	TCanvas *cnv = lvSeriesList->Canvas;
    cnv->Font->Color = clBlack;
}
//---------------------------------------------------------------------------

void __fastcall TFrameGraph::popupMenuGraphPopup(TObject *Sender)
{
    const int idx = lvSeriesList->Selected==NULL ? -1 : lvSeriesList->Selected->Index;
    if( idx>-1 && idx<chrt_->SeriesCount() )
    {
    	menuSeries->Visible = 1;
		menuSeries->Caption = MYSPRINTF_( "%d. \"%s\"", idx+1, chrt_->Series[idx]->Title );
        menuSeries->Tag = idx;
    } else
    {
    	menuSeries->Visible = 0;
    }		
}
//---------------------------------------------------------------------------



void __fastcall TFrameGraph::lvSeriesListMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	TListItem *item = lvSeriesList->GetItemAt( X, Y );
	const int idx = item==NULL ? -1 : item->Index;
    if( idx>-1 && idx<chrt_->SeriesCount() )
    	chrt_->Series[idx]->Active = item->Checked;
}
//---------------------------------------------------------------------------


void __fastcall TFrameGraph::btnBrushColorClick(TObject *Sender)
{
	ColorDialog1->Color = chrt_->Color;
    if( !ColorDialog1->Execute() ) return;
    chrt_->Color = ColorDialog1->Color;
}
//---------------------------------------------------------------------------




void __fastcall TFrameGraph::btnOpenClick(TObject *Sender)
{
	if( !OpenDialog1->Execute() ) return;

    if( ::MessageBox(Application->Handle,
    	MYSPRINTF_("Открыть файл\n\"%s\"?", OpenDialog1->FileName).c_str(),
    	"Подтверждение", MB_OKCANCEL | MB_APPLMODAL | MB_ICONQUESTION )!=IDOK )
        return;
    OpenFile( OpenDialog1->FileName );
}
//---------------------------------------------------------------------------

void __fastcall TFrameGraph::btnSaveClick(TObject *Sender)
{
	if( !SaveDialog1->Execute() ) return;
    const AnsiString ext = ExtractFileExt(SaveDialog1->FileName);

    if(  (ext==".vg") && MySaveTChartToFile( SaveDialog1->FileName, chrt_ ) )
    	SetFile( SaveDialog1->FileName );
    if(  (ext==".txt") )
    	MySaveTChartToTextFile( SaveDialog1->FileName, chrt_ );
}
//---------------------------------------------------------------------------
