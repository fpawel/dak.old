object FormCheckRele: TFormCheckRele
  Left = 430
  Top = 295
  BorderStyle = bsDialog
  Caption = #1055#1088#1086#1074#1077#1088#1082#1072' '#1089#1086#1089#1090#1086#1103#1085#1080#1103' '#1082#1086#1085#1090#1072#1082#1090#1086#1074' '#1088#1077#1083#1077
  ClientHeight = 277
  ClientWidth = 557
  Color = clWindow
  Font.Charset = RUSSIAN_CHARSET
  Font.Color = clNavy
  Font.Height = -20
  Font.Name = 'Arial'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  Position = poScreenCenter
  PixelsPerInch = 120
  TextHeight = 23
  object Button1: TButton
    Left = 368
    Top = 10
    Width = 175
    Height = 42
    Caption = #1055#1088#1080#1084#1077#1085#1080#1090#1100
    ModalResult = 1
    TabOrder = 0
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 368
    Top = 61
    Width = 174
    Height = 43
    Caption = #1054#1090#1084#1077#1085#1072
    ModalResult = 2
    TabOrder = 1
    OnClick = Button2Click
  end
  object lv1: TListView
    Left = 15
    Top = 15
    Width = 334
    Height = 242
    BorderStyle = bsNone
    Columns = <
      item
        Caption = #1056#1077#1083#1077
        Width = 128
      end
      item
        Alignment = taCenter
        Caption = #1050#1086#1085#1090#1088#1086#1083#1100
        Width = 192
      end>
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -20
    Font.Name = 'Arial'
    Font.Style = []
    MultiSelect = True
    ReadOnly = True
    ParentFont = False
    PopupMenu = PopupMenu1
    TabOrder = 2
    ViewStyle = vsReport
    OnDblClick = lv1DblClick
  end
  object PopupMenu1: TPopupMenu
    OnPopup = PopupMenu1Popup
    Left = 264
    Top = 120
    object N1: TMenuItem
      Caption = #1042#1082#1083'.'
      OnClick = N3Click
    end
    object N2: TMenuItem
      Caption = #1042#1099#1082#1083'.'
      OnClick = N3Click
    end
    object N3: TMenuItem
      Caption = #1053#1077' '#1087#1088#1086#1074#1077#1088#1103#1090#1100
      OnClick = N3Click
    end
  end
end
