//
//  ViewUtils.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-11-13.
//

import SwiftUI

#if canImport(UIKit)
  import UIKit
#elseif canImport(AppKit)
  import AppKit
#endif

enum AppIdiom {
  case phone, pad, mac, other

  static var current: Self {
    #if os(macOS)
      self.mac
    #elseif os(iOS)
      switch UIDevice.current.userInterfaceIdiom {
      case .phone:
        self.phone
      case .pad:
        self.pad
      default:
        self.other
      }
    #else
      self.other
    #endif
  }
}

// Nabbed from https://stackoverflow.com/questions/56586055/how-to-get-rgb-components-from-color-in-swiftui
extension Color {
  nonisolated var components: (red: CGFloat, green: CGFloat, blue: CGFloat, opacity: CGFloat) {
    #if canImport(UIKit)
      typealias NativeColor = UIColor
    #elseif canImport(AppKit)
      typealias NativeColor = NSColor
    #endif

    var r: CGFloat = 0
    var g: CGFloat = 0
    var b: CGFloat = 0
    var o: CGFloat = 0

    guard NativeColor(self).getRed(&r, green: &g, blue: &b, alpha: &o) else {
      return (0, 0, 0, 0)
    }

    return (r, g, b, o)
  }
}

nonisolated func getContrastingColour(backgroundColour: Color) -> Color {
  let brightness =
    backgroundColour.components.red * 0.299
    + backgroundColour.components.green * 0.587
    + backgroundColour.components.blue * 0.114
  return (brightness > 0.65) ? .black : .white
}

private struct CopyEntryContextMenu<MenuContent: View>: ViewModifier {
  let entry: Entry
  let menuContent: (Entry?, Binding<Bool>?) -> MenuContent

  func body(content: Content) -> some View {
    content
      .contextMenu {
        menuContent(entry, nil)
      }
  }
}

private struct SpeakErrorAlert: ViewModifier {
  let error: Binding<SpeakErrorState>

  func body(content: Content) -> some View {
    content
      .alert(isPresented: error.showAlert) {
        Alert(
          title: Text("Couldn't speak example!"),
          message: Text(error.wrappedValue.error),
          dismissButton: .default(Text("OK")))
      }
  }
}

private struct VoiceErrorAlert: ViewModifier {
  var presented: Binding<Bool>

  func body(content: Content) -> some View {
    content
      .alert(isPresented: presented) {
        // This warning is necessary on iOS 26 because speech synthesis is broken
        // for Mandarin if the user's Siri voice is set to Cantonese.
        Alert(
          title: Text("Mandarin voice unavailable!"),
          message: Text(
            "Mandarin is unavailable when Siri uses Cantonese. Update to a newer software version to enable."
          ),
          dismissButton: .default(Text("OK"))
        )
      }
  }
}

private struct HingeObserver: ViewModifier {
  @Binding var hasHinge: Bool

  func body(content: Content) -> some View {
    if #available(iOS 27.1, *) {
      content
        .onHingeChange { _, newContext in
          hasHinge = newContext.hinge != nil
        }
    } else {
      content
    }
  }
}

extension View {
  // MARK: - Menu for copying entry content
  func copyEntryContextMenu(entry: Entry) -> some View {
    modifier(CopyEntryContextMenu(entry: entry, menuContent: copyEntryMenu))
  }

  @ViewBuilder
  func copyEntryMenu(entry: Entry?, copied: Binding<Bool>? = nil) -> some View {
    let pasteboard = UIPasteboard.general
    Button {
      pasteboard.string = entry?.traditional
      copied?.wrappedValue = true
      Task {
        try? await Task.sleep(for: .seconds(1))
        copied?.wrappedValue = false
      }
    } label: {
      Text("Copy traditional Chinese")
    }
    Button {
      pasteboard.string = entry?.simplified
      copied?.wrappedValue = true
      Task {
        try? await Task.sleep(for: .seconds(1))
        copied?.wrappedValue = false
      }
    } label: {
      Text("Copy simplified Chinese")
    }

    Divider()

    Button {
      pasteboard.string = entry?.jyutping
      copied?.wrappedValue = true
      Task {
        try? await Task.sleep(for: .seconds(1))
        copied?.wrappedValue = false
      }
    } label: {
      Text("Copy Jyutping")
    }
    Button {
      pasteboard.string = entry?.getPhonetic(
        options: .onlyCantonese, cantoneseOptions: .prettyYale)
      copied?.wrappedValue = true
      Task {
        try? await Task.sleep(for: .seconds(1))
        copied?.wrappedValue = false
      }
    } label: {
      Text("Copy Yale")
    }
    Button {
      pasteboard.string = entry?.getPhonetic(
        options: .onlyCantonese, cantoneseOptions: .cantoneseIPA)
      copied?.wrappedValue = true
      Task {
        try? await Task.sleep(for: .seconds(1))
        copied?.wrappedValue = false
      }
    } label: {
      Text("Copy Cantonese IPA")
    }

    Divider()

    Button {
      pasteboard.string = entry?.getPhonetic(
        options: .onlyMandarin, mandarinOptions: .prettyPinyin)
      copied?.wrappedValue = true
      Task {
        try? await Task.sleep(for: .seconds(1))
        copied?.wrappedValue = false
      }
    } label: {
      Text("Copy Pinyin")
    }
    Button {
      pasteboard.string = entry?.getPhonetic(
        options: .onlyMandarin, mandarinOptions: .rawPinyin)
      copied?.wrappedValue = true
      Task {
        try? await Task.sleep(for: .seconds(1))
        copied?.wrappedValue = false
      }
    } label: {
      Text("Copy Pinyin with digits")
    }
    Button {
      pasteboard.string = entry?.getPhonetic(
        options: .onlyMandarin, mandarinOptions: .zhuyin)
      copied?.wrappedValue = true
      Task {
        try? await Task.sleep(for: .seconds(1))
        copied?.wrappedValue = false
      }
    } label: {
      Text("Copy Zhuyin")
    }
    Button {
      pasteboard.string = entry?.getPhonetic(
        options: .onlyMandarin, mandarinOptions: .mandarinIPA)
      copied?.wrappedValue = true
      Task {
        try? await Task.sleep(for: .seconds(1))
        copied?.wrappedValue = false
      }
    } label: {
      Text("Copy Mandarin IPA")
    }
  }

  // MARK: - Speaker errors and alerts
  func speakErrorAlert(error: Binding<SpeakErrorState>) -> some View {
    modifier(SpeakErrorAlert(error: error))
  }

  func voiceErrorAlert(isPresented: Binding<Bool>) -> some View {
    modifier(VoiceErrorAlert(presented: isPresented))
  }

  // MARK: - Device that app is running on
  var appIdiom: AppIdiom { AppIdiom.current }

  // MARK: - Custom searchbar behaviour
  // On iPhone, the isPresented binding is useful to determine whether
  // the search interface is active. However, on iPad, isPresented is
  // set to false as soon as the picker is tapped, which makes the
  // search interface dismiss itself immediately. This is undesirable.
  @ViewBuilder
  func customSearchable(text: Binding<String>, isPresented: Binding<Bool>)
    -> some View
  {
    switch appIdiom {
    case .phone:
      self.searchable(text: text, isPresented: isPresented, prompt: "Search")
    case .pad:
      self.searchable(text: text, prompt: "Search")
    case .mac:
      self.searchable(
        text: text,
        isPresented: isPresented,
        placement: .sidebar,
        prompt: "Search",
      )
    default:
      self.searchable(text: text, prompt: "Search")
    }
  }

  func rowLine() -> some View {
    self.frame(maxWidth: .infinity, alignment: .leading)
      .lineLimit(1)
  }

  // MARK: - Liquid glass button workaround for iOS 26
  @ViewBuilder
  func useGlassProminentButtonStyle(enabled: Bool) -> some View {
    if #available(iOS 27, *), enabled {
      self.buttonStyle(.glass)
    } else {
      self.buttonStyle(.bordered)
    }
  }

  // MARK: - Check for hinge availability
  func observeHinge(_ hasHinge: Binding<Bool>) -> some View {
    modifier(HingeObserver(hasHinge: hasHinge))
  }
}
