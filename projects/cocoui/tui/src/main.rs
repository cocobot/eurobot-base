#![feature(try_trait)]
extern crate termion;
extern crate tui;
extern crate state_manager;

mod event;

use std::io;

use std::thread;
use std::time;
use termion::event::Key;
use termion::input::MouseTerminal;
use termion::raw::IntoRawMode;
use termion::screen::AlternateScreen;
use tui::backend::{Backend, TermionBackend};
use tui::{Frame, Terminal};
use tui::layout::{Constraint, Layout, Rect};
use event::{Event, Events};
use tui::widgets::Widget;
use tui::widgets::Tabs;
use tui::widgets::Block;
use tui::widgets::Borders;
use tui::widgets::SelectableList;
use tui::style::{Color, Modifier, Style};

pub struct TabsState<'a> {
    pub titles: Vec<&'a str>,
    pub index: usize,
}

impl<'a> TabsState<'a> {
    pub fn new(titles: Vec<&'a str>) -> TabsState {
        TabsState { titles, index: 0 }
    }
    pub fn next(&mut self) {
        self.index = (self.index + 1) % self.titles.len();
    }

    pub fn previous(&mut self) {
        if self.index > 0 {
            self.index -= 1;
        } else {
            self.index = self.titles.len() - 1;
        }
    }
}

struct App<'a> {
    tabs: TabsState<'a>,
}

fn main() {
    state_manager::init(42);

    loop {
        println!("Tick !");
        thread::sleep(time::Duration::from_millis(1000));
    }

    let stdout = io::stdout().into_raw_mode().unwrap();
    let stdout = MouseTerminal::from(stdout);
    let stdout = AlternateScreen::from(stdout);
    let backend = TermionBackend::new(stdout);
    let mut terminal = Terminal::new(backend).unwrap();
    terminal.hide_cursor().unwrap();

    let events = Events::new();

    let mut app = App {
        tabs: TabsState::new(vec!["Main", "Asserv"]),
    };

    loop {
        let size = terminal.size().unwrap();

        // Draw UI
        terminal.draw(|mut f| {
            let chunks = Layout::default()
                .constraints([Constraint::Length(3), Constraint::Min(0)].as_ref())
                .split(size);
            Tabs::default()
                .block(Block::default().borders(Borders::ALL).title("Tabs"))
                .titles(&app.tabs.titles)
                .style(Style::default().fg(Color::Green))
                .highlight_style(Style::default().fg(Color::Yellow))
                .select(app.tabs.index)
                .render(&mut f, chunks[0]);
            match app.tabs.index {
                0 => draw_main_tab(&mut f, &app, chunks[1]),
                _ => {}
            };
        }).unwrap();

        match events.next().unwrap() {
            Event::Input(input) => match input {
                Key::Char('q') => {
                    break;
                }
                //Key::Up => {
                //    if app.selected > 0 {
                //        app.selected -= 1
                //    };
                //}
                //Key::Down => if app.selected < app.items.len() - 1 {
                //    app.selected += 1;
                //},
                Key::Left => {
                    app.tabs.previous();
                }
                Key::Right => {
                    app.tabs.next();
                }
                //Key::Char('t') => {
                //    app.show_chart = !app.show_chart;
                //}
                _ => {}
            },
            Event::Tick => {
            //    app.progress += 5;
            //    if app.progress > 100 {
            //        app.progress = 0;
            //    }
            //    app.data.insert(0, rand_signal.next().unwrap());
            //    app.data.pop();
            //    for _ in 0..5 {
            //        app.data2.remove(0);
            //        app.data2.push(sin_signal.next().unwrap());
            //    }
            //    for _ in 0..10 {
            //        app.data3.remove(0);
            //        app.data3.push(sin_signal2.next().unwrap());
            //    }
            //    let i = app.data4.pop().unwrap();
            //    app.data4.insert(0, i);
            //    app.window[0] += 1.0;
            //    app.window[1] += 1.0;
            //    let i = app.events.pop().unwrap();
            //    app.events.insert(0, i);
            //    app.color_index += 1;
            //    if app.color_index >= app.colors.len() {
            //        app.color_index = 0;
            //    }
            }
        }
    }
}

fn draw_main_tab<B>(f: &mut Frame<B>, app: &App, area: Rect)
where
    B: Backend,
{
    let chunks = Layout::default()
        .constraints(
            [
            Constraint::Length(7),
            Constraint::Min(7),
            Constraint::Length(7),
            ]
            .as_ref(),
            ).split(area);
    SelectableList::default()
        .block(Block::default().borders(Borders::ALL).title("Board list"))
        .highlight_style(Style::default().fg(Color::Yellow).modifier(Modifier::Bold))
        .highlight_symbol(">")
        .render(f, chunks[0]);
}
