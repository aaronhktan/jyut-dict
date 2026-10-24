//
//  LicenceView.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-10-24.
//

import SwiftUI
import os

struct LicenceView: View {
  @State var licences = Licences()

  var body: some View {
    NavigationStack {
      List {
        NavigationLink {
          ScrollView {
            Text(licences.MLKitLicence)
              .font(.caption.monospaced(true))
              .padding()
          }
        } label: {
          Text("Google ML Kit")
        }

        NavigationLink {
          ScrollView {
            Text(licences.JyutDictionaryLicence)
              .font(.caption.monospaced(true))
              .padding()
          }
        } label: {
          Text("Jyut Dictionary")
        }
      }
    }
    .navigationTitle("Code Licences")
  }
}

@Observable
class Licences {
  var MLKitLicence: String = ""
  var JyutDictionaryLicence: String = ""

  @ObservationIgnored private var MLKitLicenceFilePath = Bundle.main.url(
    forResource: "MLKitLicence",
    withExtension: "txt"
  )!

  @ObservationIgnored private var JyutDictionaryLicenceFilePath = Bundle.main.url(
    forResource: "JyutDictionaryLicence",
    withExtension: "txt"
  )!

  init() {
    MLKitLicence = self.load(MLKitLicenceFilePath)
    JyutDictionaryLicence = self.load(JyutDictionaryLicenceFilePath)
  }

  func load(_ url: URL) -> String {
    do {
      return try String(contentsOf: url, encoding: .utf8)
    } catch {
      logger.error("Failed to load licence at \(url), error: \(error))")
    }

    return ""
  }
}
