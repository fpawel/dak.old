object FormAppSetsDlg: TFormAppSetsDlg
  Left = 243
  Top = 125
  Width = 765
  Height = 572
  Caption = #1055#1072#1088#1072#1084#1077#1090#1088#1099' '#1087#1088#1080#1083#1086#1078#1077#1085#1080#1103
  Color = clWindow
  Font.Charset = RUSSIAN_CHARSET
  Font.Color = clWindowText
  Font.Height = -15
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnActivate = FormActivate
  PixelsPerInch = 120
  TextHeight = 18
  object Panel2: TPanel
    Left = 0
    Top = 0
    Width = 228
    Height = 527
    Align = alLeft
    BevelOuter = bvNone
    ParentColor = True
    TabOrder = 4
    object RadioButton1: TRadioButton
      Left = 10
      Top = 12
      Width = 197
      Height = 20
      Caption = #1055#1088#1080#1105#1084#1086#1087#1077#1088#1077#1076#1072#1095#1072
      Checked = True
      TabOrder = 0
      TabStop = True
      OnClick = RadioButton1Click
    end
    object RadioButton2: TRadioButton
      Tag = 2
      Left = 10
      Top = 78
      Width = 197
      Height = 20
      Caption = #1044#1086#1087#1086#1083#1085#1080#1090#1077#1083#1100#1085#1086
      TabOrder = 1
      OnClick = RadioButton1Click
    end
    object RadioButton3: TRadioButton
      Tag = 1
      Left = 12
      Top = 44
      Width = 196
      Height = 22
      Caption = #1044#1072#1085#1085#1099#1077' '#1044#1040#1050'-'#1052
      TabOrder = 2
      OnClick = RadioButton1Click
    end
  end
  object grd1: TStringGrid
    Left = 228
    Top = 0
    Width = 317
    Height = 527
    Align = alClient
    BorderStyle = bsNone
    ColCount = 3
    DefaultColWidth = 150
    DefaultRowHeight = 23
    FixedCols = 0
    GridLineWidth = 0
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goColSizing, goEditing]
    TabOrder = 0
    OnSelectCell = grd1SelectCell
    OnSetEditText = grd1SetEditText
    ColWidths = (
      173
      77
      116)
  end
  object cbPortName: TComboBox
    Left = 28
    Top = 123
    Width = 185
    Height = 24
    Style = csOwnerDrawVariable
    ItemHeight = 18
    TabOrder = 1
    OnKeyDown = cbChanalKeyDown
  end
  object edGrd: TEdit
    Left = 28
    Top = 320
    Width = 144
    Height = 22
    TabOrder = 2
    OnKeyDown = cbChanalKeyDown
  end
  object ComboBox1: TComboBox
    Left = 28
    Top = 183
    Width = 180
    Height = 26
    Style = csDropDownList
    DropDownCount = 100
    ItemHeight = 18
    TabOrder = 3
    OnKeyDown = cbChanalKeyDown
    Items.Strings = (
      #1044#1072
      #1053#1077#1090)
  end
  object cbGas: TComboBox
    Left = 274
    Top = 183
    Width = 180
    Height = 26
    Style = csDropDownList
    DropDownCount = 100
    ItemHeight = 18
    TabOrder = 5
    OnKeyDown = cbChanalKeyDown
    Items.Strings = (
      'CO2'
      'CH'
      'C3H8')
  end
  object cbScale: TComboBox
    Left = 265
    Top = 251
    Width = 179
    Height = 26
    Style = csDropDownList
    DropDownCount = 100
    ItemHeight = 18
    TabOrder = 6
    OnKeyDown = cbChanalKeyDown
    Items.Strings = (
      '4'
      '10'
      '20'
      '100')
  end
  object Panel1: TPanel
    Left = 545
    Top = 0
    Width = 202
    Height = 527
    Align = alRight
    BevelOuter = bvNone
    ParentColor = True
    TabOrder = 7
    object Label1: TLabel
      Left = 51
      Top = 144
      Width = 105
      Height = 18
      Caption = #1082#1086#1101#1092#1092#1080#1094#1080#1077#1085#1090#1099
    end
    object Button1: TButton
      Left = 10
      Top = 8
      Width = 180
      Height = 45
      Caption = #1055#1088#1080#1084#1077#1085#1080#1090#1100
      TabOrder = 0
      OnClick = Button1Click
    end
    object Button2: TButton
      Left = 10
      Top = 59
      Width = 180
      Height = 45
      Caption = #1054#1090#1084#1077#1085#1072
      ModalResult = 2
      TabOrder = 1
    end
    object CheckBox1: TCheckBox
      Left = 26
      Top = 114
      Width = 140
      Height = 22
      Caption = #1059#1089#1090#1072#1085#1086#1074#1080#1090#1100
      Checked = True
      State = cbChecked
      TabOrder = 2
    end
  end
end
