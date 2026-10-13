//
//  HandwritingView.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-08.
//

import MLKitDigitalInkRecognition
import SwiftUI
import os

struct HandwritingView: View {
  @Environment(\.dismiss) var dismiss

  @Bindable var searchContext: SearchContext

  @Binding var isSearchActive: Bool

  @State private var canvasState = CanvasState()
  @State private var searchText = ""
  @State private var buttonText = "Search"

  @State private var modelIsAvailable = false
  @State private var showModelDownloadFailure = false

  let lineWidth = 5.0
  let epsilon = 5.0

  var body: some View {
    NavigationStack {
      VStack {
        if !modelIsAvailable {
          ContentUnavailableView {
            ProgressView()
              .scaleEffect(2.0)
              .padding()
            Text("Downloading and installing handwriting model...")
          }
        } else {
          CanvasView()
          CandidatesView()
          Spacer()
          ButtonView()
        }
      }
      .toolbar {
        #if os(iOS)
          ToolbarItem(placement: .topBarTrailing) {
            Button("Close handwriting view", systemImage: "xmark") {
              dismiss()
            }
            .labelsHidden()
          }
        #else
          Button("Close handwriting view", systemImage: "xmark") {
            dismiss()
          }
        #endif
      }
    }
    .task {
      let nc = NotificationCenter.default
      nc.addObserver(forName: NSNotification.Name.mlkitModelDownloadDidSucceed, object: nil, queue: nil, using: { _ in
        Task { @MainActor in
          logger.info("Model download completed")
          modelIsAvailable = true
        }
      })
      nc.addObserver(forName: NSNotification.Name.mlkitModelDownloadDidFail, object: nil, queue: nil, using: { _ in
        Task { @MainActor in
          logger.error("Model download failed")
          showModelDownloadFailure = true
        }
      })
      let languageTag = "zh-Hani-HK"
      let identifier = DigitalInkRecognitionModelIdentifier(forLanguageTag: languageTag)
      guard let identifier else {
        logger.error("Couldn't find identifier: \(languageTag)!")
        showModelDownloadFailure = true
        return
      }
      let model = DigitalInkRecognitionModel(modelIdentifier: identifier)
      let modelManager = ModelManager.modelManager()
      if modelManager.isModelDownloaded(model) {
        modelIsAvailable = true
      } else {
        let conditions = ModelDownloadConditions.init(
          allowsCellularAccess: true,
          allowsBackgroundDownloading: false)
        modelManager.download(model, conditions: conditions)
      }
      canvasState.recognizer = DigitalInkRecognizer.digitalInkRecognizer(
        options: DigitalInkRecognizerOptions(model: model))
    }
    .alert(
      "Jyut Dictionary could not locate the handwriting model.",
      isPresented: $showModelDownloadFailure
    ) {
      Button("OK", role: .cancel) {
        dismiss()
      }
    }
  }

  private func CGPointDistanceSquared(from: CGPoint?, to: CGPoint?) -> CGFloat? {
    if let from, let to {
      return (from.x - to.x) * (from.x - to.x) + (from.y - to.y) * (from.y - to.y)
    }

    return nil
  }
}

extension HandwritingView {
  // MARK: - Canvas
  @ViewBuilder
  private func CanvasView() -> some View {
    Canvas { context, size in
      for path in canvasState.paths {
        if path.isFilled {
          context.fill(path.path, with: .color(.primary))
        } else {
          context.stroke(path.path, with: .color(.primary), lineWidth: lineWidth)
        }
      }
      context.stroke(canvasState.currentPath, with: .color(.primary), lineWidth: lineWidth)
    }
    .background(.quinary)
    .aspectRatio(CGSize(width: 1, height: 1), contentMode: .fit)
    .clipShape(
      .rect(
        topLeadingRadius: 20,
        bottomLeadingRadius: 20,
        bottomTrailingRadius: 20,
        topTrailingRadius: 20
      )
    )
    .padding([.horizontal])
    .gesture(
      DragGesture(minimumDistance: 0, coordinateSpace: .local)
        .onChanged { value in
          if canvasState.currentPath.isEmpty {
            canvasState.currentPath.move(to: value.startLocation)
            canvasState.startLocation = value.startLocation
          } else {
            canvasState.currentPath.addLine(to: value.location)
          }

          canvasState.points.append(
            StrokePoint.init(
              x: Float(value.location.x),
              y: Float(value.location.y),
              t: Int(Date.now.timeIntervalSince1970 * 1000)))
        }
        .onEnded { value in
          if CGPointDistanceSquared(from: canvasState.startLocation, to: value.location) ?? 0
            < (epsilon * epsilon)
          {
            canvasState.currentPath.addEllipse(
              in: CGRect(x: value.location.x - 3, y: value.location.y - 3, width: 6, height: 6))
            canvasState.paths.append((path: canvasState.currentPath, isFilled: true))
          } else {
            canvasState.paths.append((path: canvasState.currentPath, isFilled: false))
          }

          canvasState.strokes.append(Stroke.init(points: canvasState.points))
          canvasState.points = []
          canvasState.currentPath = Path()
          canvasState.startLocation = nil

          canvasState.doRecognition()
        }
    )
  }

  // MARK: - Candidates
  @ViewBuilder
  private func CandidatesView() -> some View {
    ScrollView(.horizontal) {
      HStack {
        ForEach((0...9), id: \.self) { index in
          if canvasState.candidates.count > index {
            Button {
              searchText.append(canvasState.candidates[index])
              buttonText = "Search for: \(searchText)"
              canvasState.clear()
            } label: {
              Text(canvasState.candidates[index])
                .lineLimit(1)
                .truncationMode(.middle)
            }
            .font(.title)
            .padding(.horizontal)
            .glassEffect()
          } else {
            // Just here to reserve space
            Button("大", action: {})
              .font(.title)
              .padding(.horizontal)
              .glassEffect()
              .hidden()
          }
        }
      }
    }
    .scrollIndicators(.hidden)
    .padding()
  }

  // MARK: - Buttons
  @ViewBuilder
  private func ButtonView() -> some View {
    Button {
      canvasState.clear()
    } label: {
      Text("Reset canvas")
        .lineLimit(1)
        .truncationMode(.middle)
        .frame(maxWidth: .infinity)
    }
    .buttonStyle(.bordered)
    .buttonBorderShape(.capsule)
    .controlSize(.extraLarge)
    .padding(.horizontal)
    .disabled(canvasState.paths.isEmpty)

    Button {
      searchText = String(searchText.dropLast())
      if searchText.isEmpty {
        buttonText = "Search"
      } else {
        buttonText = "Search for: \(searchText)"
      }
    } label: {
      Text("Backspace")
        .frame(maxWidth: .infinity)
    }
    .buttonStyle(.bordered)
    .buttonBorderShape(.capsule)
    .controlSize(.extraLarge)
    .padding(.horizontal)
    .disabled(searchText == "")

    Button {
      if !searchText.isEmpty {
        searchContext.searchText = searchText
        searchContext.selectedOption = .autoDetect
        isSearchActive = true
      }
      dismiss()
    } label: {
      Text(buttonText)
        .lineLimit(1)
        .truncationMode(.middle)
        .frame(maxWidth: .infinity)
        .contentTransition(.opacity)
        .animation(.easeInOut(duration: 0.3), value: buttonText)
    }
    .buttonStyle(.borderedProminent)
    .buttonBorderShape(.capsule)
    .controlSize(.extraLarge)
    .padding(.horizontal)
    .disabled(searchText == "")
  }
}

@Observable @MainActor final class CanvasState {
  // For drawing on canvas
  var currentPath: Path = Path()
  var startLocation: CGPoint?
  var paths: [(path: Path, isFilled: Bool)] = []

  // For MLKit
  var recognizer: DigitalInkRecognizer? = nil
  var strokes: [Stroke] = []
  var points: [StrokePoint] = []

  // For character display
  var candidates: [String] = []

  func doRecognition() {
    let ink = Ink.init(strokes: strokes)
    recognizer?.recognize(
      ink: ink,
      completion: { (result: DigitalInkRecognitionResult?, error: Error?) in
        guard let result else { return }
        let texts = result.candidates.map(\.text)
        Task { @MainActor in
          self.candidates = texts
        }
      }
    )
  }

  func clear() {
    currentPath = Path()
    startLocation = nil
    paths = []

    strokes = []
    points = []

    candidates = []
  }
}
