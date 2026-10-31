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
  let appBuild = Bundle.main.infoDictionary?["CFBundleVersion"] as? String
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

        Text("Build \(appVersion ?? "0.0") (\(appBuild ?? "0"))")
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

        HStack(spacing: 5) {
          Button {
            openURL(websiteURL)
          } label: {
            VStack(spacing: 7) {
              Image(systemName: "globe")
              Text("Visit website")
                .font(.caption)
            }
            .padding(.vertical, 5)
            .frame(maxWidth: .infinity)
          }
          .buttonStyle(.bordered)
          .tint(.accent)
          .buttonBorderShape(.roundedRectangle)

          Button {
            openURL(githubURL)
          } label: {
            VStack(spacing: 7) {
              Image(systemName: "chevron.left.forwardslash.chevron.right")
              Text("GitHub repo")
                .font(.caption)
            }
            .padding(.vertical, 5)
            .frame(maxWidth: .infinity)
          }
          .buttonStyle(.bordered)
          .tint(.accent)
          .buttonBorderShape(.roundedRectangle)

          Button {
            openURL(emailURL)
          } label: {
            VStack(spacing: 7) {
              Image(systemName: "envelope")
              Text("Say hi!")
                .font(.caption)
            }
            .padding(.vertical, 5)
            .frame(maxWidth: .infinity)
          }
          .buttonStyle(.bordered)
          .tint(.accent)
          .buttonBorderShape(.roundedRectangle)
        }
        .padding(.horizontal)

        Spacer()
      }
    }
  }
}
