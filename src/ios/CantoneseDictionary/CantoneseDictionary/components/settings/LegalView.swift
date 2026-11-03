//
//  LegalView.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-10-24.
//

import SwiftUI
import os

struct LegalView: View {
  @State var documents = Documents()

  var body: some View {
    NavigationStack {
      List {
        NavigationLink {
          ScrollView {
            Text(documents.MLKitLicence)
              .font(.caption.monospaced(true))
              .padding()
          }
        } label: {
          Text("Google ML Kit")
        }

        NavigationLink {
          ScrollView {
            Text(documents.JyutDictionaryLicence)
              .font(.caption.monospaced(true))
              .padding()
          }
        } label: {
          Text("Jyut Dictionary Licence")
        }

        NavigationLink {
          ScrollView {
            Text(documents.PrivacyPolicy)
              .font(.caption.monospaced(true))
              .padding()
          }
        } label: {
          Text("Jyut Dictionary Privacy Policy")
        }
      }
    }
    .contentMargins(.top, 0)
    .navigationTitle("Legal")
  }
}

@Observable
class Documents {
  var MLKitLicence: String = ""
  var JyutDictionaryLicence: String = ""
  var PrivacyPolicy: String = ""

  @ObservationIgnored private var MLKitLicenceFilePath = Bundle.main.url(
    forResource: "MLKitLicence",
    withExtension: "txt"
  )!

  @ObservationIgnored private var JyutDictionaryLicenceFilePath = Bundle.main.url(
    forResource: "JyutDictionaryLicence",
    withExtension: "txt"
  )!

  @ObservationIgnored private var PrivacyPolicyFilePath = Bundle.main.url(
    forResource: "PrivacyPolicy",
    withExtension: "txt"
  )!

  init() {
    MLKitLicence = self.load(MLKitLicenceFilePath)
    JyutDictionaryLicence = self.load(JyutDictionaryLicenceFilePath)
    PrivacyPolicy = self.load(PrivacyPolicyFilePath)
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
