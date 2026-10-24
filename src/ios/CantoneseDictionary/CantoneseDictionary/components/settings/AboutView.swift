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
  let developerURL = URL(string: "https://www.aaronhktan.com")!
  let websiteURL = URL(string: "https://www.jyutdictionary.com")!
  let githubURL = URL(string: "https://www.github.com/aaronhktan/jyut-dict")!
  let emailURL = URL(string: "mailto:help@jyutdictionary.com")!

  var body: some View {
    ScrollView {
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

        ZStack {
          RoundedRectangle(cornerRadius: 20)
            .fill(.quinary)

          Label {
            Text("Made with ❤︎ by Aaron Tan")
          } icon: {
            Image("DeveloperImage")
              .resizable()
              .scaledToFit()
              .frame(width: 50)
              .onTapGesture {
                openURL(developerURL)
              }
          }
          .padding()
        }
        .padding()

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
          Label("GitHub repo", systemImage: "chevron.left.forwardslash.chevron.right")
            .frame(maxWidth: .infinity)
        }
        .buttonStyle(.glass)
        .tint(.accent)
        .buttonBorderShape(.capsule)
        .controlSize(.extraLarge)
        .padding(.horizontal)

        Button {
          openURL(emailURL)
        } label: {
          Label("Report an issue or just say hi!", systemImage: "envelope")
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
}
