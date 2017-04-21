object FormPneumoGrd: TFormPneumoGrd
  Left = 451
  Top = 187
  Width = 348
  Height = 282
  Caption = 'FormPneumoGrd'
  Color = clWindow
  Font.Charset = RUSSIAN_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 14
  object grdPneumo: TStringGrid
    Left = 15
    Top = 16
    Width = 298
    Height = 201
    BorderStyle = bsNone
    Color = clWhite
    ColCount = 7
    DefaultColWidth = 5
    DefaultRowHeight = 25
    FixedColor = cl3DLight
    RowCount = 10
    FixedRows = 0
    GridLineWidth = 0
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goColSizing]
    TabOrder = 0
    OnKeyDown = grdPneumoKeyDown
    OnMouseMove = grdPneumoMouseMove
    OnSelectCell = grdPneumoSelectCell
  end
  object Timer1: TTimer
    Left = 88
    Top = 48
  end
end
