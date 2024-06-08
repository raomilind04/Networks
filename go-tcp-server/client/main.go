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
    prog := tea.NewProgram(initModel(), tea.WithAltScreen())
    if _, err := prog.Run(); err != nil {
        log.Fatal(err)
    }
}

type applicationState int

const (
    stateInputName = iota
    stateConfirmName
    stateChat
)

type model struct {
    applicationState applicationState
    name             string
    viewport         viewport.Model
    textarea         textarea.Model
    msgs             []string
    styles           lipgloss.Style
    err              error
    confirmSelected  bool
}

func initModel() model {
    ta := textarea.New()
    ta.Placeholder = "Enter your name"
    ta.Focus()

    ta.Prompt = "┃ "

    ta.SetWidth(40)
    ta.SetHeight(1)

    ta.FocusedStyle.CursorLine = lipgloss.NewStyle().Foreground(lipgloss.Color("5"))   

    ta.ShowLineNumbers = false
    ta.KeyMap.InsertNewline.SetEnabled(false)

    view := viewport.New(40, 7)
    view.SetContent("Hello !")


    return model {
        applicationState: stateInputName,
        textarea:         ta,
        viewport:         view,
        msgs:             []string{},
        styles:           lipgloss.NewStyle().Foreground(lipgloss.Color("5")),
        err:              nil,
        confirmSelected:  false,    
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
    switch m.applicationState {
    case stateInputName:
        m.textarea, textcmd = m.textarea.Update(msg)   
        switch msg := msg.(type) {
        case tea.KeyMsg:
            switch msg.Type {
            case tea.KeyEnter:
                m.name = m.textarea.Value()
                m.applicationState = stateConfirmName
            case tea.KeyCtrlC:
                return m, tea.Quit
            }
        }
    case stateConfirmName:
        switch msg := msg.(type) {
        case tea.KeyMsg:
            switch msg.Type {
            case tea.KeyEnter:
                if m.confirmSelected {
                    m.applicationState = stateChat
                    m.textarea.Reset()
                    m.textarea.Placeholder = ""
                    m.viewport.SetContent(fmt.Sprintf("Welcome, %s", m.name))
                } else {
                    m.applicationState = stateInputName
                    m.textarea.Reset()
                }
            case tea.KeyTab:
                m.confirmSelected = !m.confirmSelected
            case tea.KeyCtrlC:
                return m, tea.Quit
            }
        }
    case stateChat:
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
            case tea.KeyCtrlC:
                fmt.Println(m.textarea.Value())
                return m, tea.Quit
            }
        case error:
            m.err = msg
            return m, nil
        }
    }

    return m, tea.Batch(textcmd, viewcmd)
}

func (m model) View() string {
    switch m.applicationState {
    case stateInputName:
        return m.textarea.View() + "\n\nPress Enter to confirm"
    case stateConfirmName:
        confirmStyle := lipgloss.NewStyle().Foreground(lipgloss.Color("2")).Bold(true)
        cancelStyle := lipgloss.NewStyle().Foreground(lipgloss.Color("1")).Bold(true)
        if m.confirmSelected {
        return fmt.Sprintf("You entered: %s\n%s    %s",
            m.name,
            confirmStyle.Render("[Confirm]"),
            cancelStyle.Render("Cancel"))
		} else {
			return fmt.Sprintf("You entered: %s\n%s    %s",
				m.name,
				confirmStyle.Render("Confirm"),
				cancelStyle.Render("[Cancel]"))
		}
    case stateChat:
        return fmt.Sprintf(
            "%s\n\n%s",
            m.viewport.View(),
            m.textarea.View(),
        ) + "\n\n"
    default:
        panic("Unknown State")
    }
}
