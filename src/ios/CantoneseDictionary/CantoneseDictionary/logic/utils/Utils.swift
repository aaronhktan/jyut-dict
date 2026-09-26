//
//  Utils.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-09.
//

import Foundation
import SwiftUI
import os

#if canImport(UIKit)
  import UIKit
#elseif canImport(AppKit)
  import AppKit
#endif

nonisolated let sameCharacterString = "－"
nonisolated let logger = Logger()
nonisolated let iso693ToLanguageName: [String: String] = [
  "cmn": "Mandarin",
  "deu": "German",
  "eng": "English",
  "fra": "French",
  "yue": "Cantonese",
  "zh": "Chinese",
]

// Nabbed from https://forums.swift.org/t/find-multiple-substrings-from-specified-index/28667/4
extension String {
  nonisolated func firstIndex(of: String, at: String.Index) -> String.Index? {
    self[at...].range(of: of)?.lowerBound
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

extension View {
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
}
