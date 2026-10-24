//
//  AboutView.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-10-24.
//

import SwiftUI

struct AboutView: View {
  @Environment(\.openURL) var openURL

  let appVersion = Bundle.main.infoDictionary?["CFBundleShortVersionString"] as? String
  let websiteURL = URL(string: "https://www.jyutdictionary.com")!
  let githubURL = URL(string: "https://www.github.com/aaronhktan/jyut-dict")!

  var body: some View {
    VStack {
      Label {
        Text("App Icon")
      } icon: {
        Image("AppIconImage")
          .resizable()
          .scaledToFit()
          .frame(width: 200)
      }
      .labelStyle(.iconOnly)

      Text("Jyut Dictionary")
        .font(.title)
        .fontWeight(.bold)
        .dynamicTypeSize(.xxxLarge)
        .padding(.horizontal)

      Text("Build \(appVersion ?? "0.0")")
        .font(.subheadline)
        .foregroundStyle(Color.secondary)

      Text("A free, open-source, offline-first Cantonese dictionary.")
        .font(.body)
        .multilineTextAlignment(.center)
        .padding(.horizontal)

      Button {
        openURL(websiteURL)
      } label: {
        Label("Visit website", systemImage: "globe")
          .frame(maxWidth: .infinity)
      }
      .buttonStyle(.glass)
      .tint(.accent)
      .buttonBorderShape(.capsule)
      .controlSize(.extraLarge)
      .padding(.horizontal)

      Button {
        openURL(githubURL)
      } label: {
        Label("GitHub repo", systemImage: "globe")
          .frame(maxWidth: .infinity)
      }
      .buttonStyle(.glass)
      .tint(.accent)
      .buttonBorderShape(.capsule)
      .controlSize(.extraLarge)
      .padding(.horizontal)

      Spacer()
    }
  }
}
