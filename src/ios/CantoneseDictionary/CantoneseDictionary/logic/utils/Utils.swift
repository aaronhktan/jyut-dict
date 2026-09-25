//
//  Utils.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-09.
//

import Foundation
import SwiftUI
import os

nonisolated let sameCharacterString = "－"
nonisolated let logger = Logger()

// Nabbed from https://forums.swift.org/t/find-multiple-substrings-from-specified-index/28667/4
extension String {
  nonisolated func firstIndex(of: String, at: String.Index) -> String.Index? {
    self[at...].range(of: of)?.lowerBound
  }
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
