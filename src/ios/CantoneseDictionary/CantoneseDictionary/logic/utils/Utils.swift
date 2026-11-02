//
//  Utils.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-09.
//

import AVFoundation
import Foundation
import SwiftUI
import os

#if canImport(UIKit)
  import UIKit
#elseif canImport(AppKit)
  import AppKit
#endif

nonisolated let sameCharacterString = "－"
nonisolated let logger = Logger(
  subsystem: Bundle.main.bundleIdentifier!,
  category: String(describing: LogStore.self)
)
nonisolated let iso693ToLanguageName: [String: String] = [
  "cmn": "Mandarin",
  "deu": "German",
  "eng": "English",
  "fra": "French",
  "yue": "Cantonese",
  "zh": "Chinese",
  "zho": "Chinese",
]

nonisolated func unfoldBasicRegex(for pattern: String) -> [String] {
  var out: [String] = []
  var stringPossibilities: [String] = []

  // Invariant: there must be only one set of parentheses and one question mark
  if pattern.count(where: { $0 == "(" }) > 1
    || pattern.count(where: { $0 == ")" }) > 1
    || pattern.count(where: { $0 == "?" }) > 1
  {
    return out
  }

  // If there is a parenthesis, then all of the options must be checked
  if pattern.firstIndex(of: "(") != nil
    && pattern.firstIndex(of: ")") != nil
  {
    let openParenIdx = pattern.firstIndex(of: "(")
    var startIdx = pattern.index(after: openParenIdx!)
    let endIdx = pattern.firstIndex(of: ")")
    var orIdx = pattern.firstIndex(of: "|")

    while orIdx != nil {
      let preParen = pattern[..<openParenIdx!]
      let possibility = pattern[startIdx..<orIdx!]
      let postParen = pattern[pattern.index(after: endIdx!)...]
      stringPossibilities.append(
        String(preParen + possibility + postParen)
      )
      startIdx = pattern.index(after: orIdx!)
      orIdx = pattern.firstIndex(of: "|", at: startIdx)
    }
    let preParen = pattern[..<openParenIdx!]
    let possibility = pattern[startIdx..<endIdx!]
    let postParen = pattern[pattern.index(after: endIdx!)...]
    stringPossibilities.append(
      String(preParen + possibility + postParen)
    )
  } else {
    stringPossibilities.append(pattern)
  }

  // If there is a "?", then the initial with and the initial without
  // that optional character should be considered
  for s in stringPossibilities {
    let regexIdx = s.firstIndex(of: "?")
    if regexIdx == nil {
      out.append(s)
    } else if regexIdx == s.startIndex {
      out.append(String(s[s.index(after: s.startIndex)...]))
    } else {
      var tmp = s
      tmp.removeSubrange(
        s.index(before: regexIdx!)..<s.index(after: regexIdx!)
      )
      out.append(tmp)
      tmp = s
      tmp.remove(at: regexIdx!)
      out.append(tmp)
    }
  }

  return out
}

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

  // MARK: - Liquid glass button workaround for iOS 26
  @ViewBuilder
  func useGlassProminentButtonStyle(enabled: Bool) -> some View {
    if #available(iOS 27, *), enabled {
      self.buttonStyle(.glass)
    } else {
      self.buttonStyle(.bordered)
    }
  }
}

extension AVAudioPCMBuffer {
  nonisolated func deepCopy() -> AVAudioPCMBuffer? {
    guard let dest = AVAudioPCMBuffer(pcmFormat: format, frameCapacity: frameLength) else {
      return nil
    }
    dest.frameLength = frameLength

    let src = UnsafeMutableAudioBufferListPointer(mutableAudioBufferList)
    let dst = UnsafeMutableAudioBufferListPointer(dest.mutableAudioBufferList)
    for (s, d) in zip(src, dst) {
      guard let sData = s.mData, let dData = d.mData else {
        return nil
      }
      memcpy(dData, sData, Int(s.mDataByteSize))
    }
    return dest
  }
}
