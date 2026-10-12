//
//  HandwritingView.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-08.
//

import SwiftUI

struct HandwritingView: View {
  @Environment(\.dismiss) var dismiss

  @State private var currentPath: Path = Path()
  @State private var startLocation: CGPoint?
  @State private var paths: [(path: Path, isFilled: Bool)] = []

  let lineWidth = 5.0
  let epsilon = 5.0

  var body: some View {
    NavigationStack {
      VStack {
        Canvas { context, size in
          for path in paths {
            if path.isFilled {
              context.fill(path.path, with: .color(.primary))
            } else {
              context.stroke(path.path, with: .color(.primary), lineWidth: lineWidth)
            }
          }
          context.stroke(currentPath, with: .color(.primary), lineWidth: lineWidth)
        }
        .background(.quinary)
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
              if currentPath.isEmpty {
                currentPath.move(to: value.startLocation)
                startLocation = value.startLocation
              } else {
                currentPath.addLine(to: value.location)
              }
            }
            .onEnded { value in
              if CGPointDistanceSquared(from: startLocation, to: value.location) ?? 0
                < (epsilon * epsilon)
              {
                currentPath.addEllipse(
                  in: CGRect(x: value.location.x - 3, y: value.location.y - 3, width: 6, height: 6))
                paths.append((path: currentPath, isFilled: true))
              } else {
                paths.append((path: currentPath, isFilled: false))
              }
              currentPath = Path()
              startLocation = nil
            }
        )
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
  }

  func CGPointDistanceSquared(from: CGPoint?, to: CGPoint?) -> CGFloat? {
    if let from, let to {
      return (from.x - to.x) * (from.x - to.x) + (from.y - to.y) * (from.y - to.y)
    }

    return nil
  }
}

#Preview {
  HandwritingView()
}
