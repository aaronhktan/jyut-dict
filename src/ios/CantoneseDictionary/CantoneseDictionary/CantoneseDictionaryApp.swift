//
//  CantoneseDictionaryApp.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-02.
//

import SwiftUI

@main
struct CantoneseDictionaryApp: App {
  @State var databaseManager = DatabaseManager()
  @State var settings = Settings()

  var body: some Scene {
    WindowGroup {
      ContentView()
        .environment(databaseManager)
        .environment(settings)
        .tint(.accent)
    }
  }
}
