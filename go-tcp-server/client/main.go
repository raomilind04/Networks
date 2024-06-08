package main

import (
	"fmt"
	"log"
	"strings"

	"github.com/charmbracelet/bubbles/textarea"
	"github.com/charmbracelet/bubbles/viewport"
	tea "github.com/charmbracelet/bubbletea"
	"github.com/charmbracelet/lipgloss"
) 

func main() {
    prog := tea.NewProgram(initModel())
    if _, err := prog.Run(); err != nil {
        log.Fatal(err)
    }
}

type model struct {
    viewport viewport.Model
    textarea textarea.Model
    msgs      []string
    styles   lipgloss.Style
    err      error
}

func initModel() model {
    ta := textarea.New()
    ta.Placeholder = ""
    ta.Focus()

    ta.Prompt = "┃ "

    ta.SetWidth(40)
    ta.SetHeight(15)

    ta.FocusedStyle.CursorLine = lipgloss.NewStyle()

    ta.ShowLineNumbers = false
    ta.KeyMap.InsertNewline.SetEnabled(false)

    view := viewport.New(40, 7)
    view.SetContent("Hello !")

    return model {
        textarea: ta,
        viewport: view,
        msgs:      []string{},
        styles:   lipgloss.NewStyle().Foreground(lipgloss.Color("5")),
        err:      nil,
    }

}

func (m model) Init() tea.Cmd {
    return textarea.Blink
}

func (m model) Update(msg tea.Msg) (tea.Model, tea.Cmd) {
    var (
        textcmd tea.Cmd
        viewcmd tea.Cmd
    )

    m.textarea, textcmd = m.textarea.Update(msg)
    m.viewport, viewcmd = m.viewport.Update(msg)

    switch msg := msg.(type) {
    case tea.KeyMsg:
        switch msg.Type {
        case tea.KeyEnter:
            m.msgs = append(m.msgs, m.styles.Render("You: ")+m.textarea.Value())
			m.viewport.SetContent(strings.Join(m.msgs, "\n"))
			m.textarea.Reset()
			m.viewport.GotoBottom()
        }
    case error:
        m.err = msg
        return m, nil
    }
    return m, tea.Batch(textcmd, viewcmd)
}

func (m model) View() string {
	return fmt.Sprintf(
		"%s\n\n%s",
		m.viewport.View(),
		m.textarea.View(),
	) + "\n\n"
}
