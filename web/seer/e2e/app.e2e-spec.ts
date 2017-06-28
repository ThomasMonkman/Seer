import { SeerPage } from './app.po';

describe('seer App', () => {
  let page: SeerPage;

  beforeEach(() => {
    page = new SeerPage();
  });

  it('should display welcome message', () => {
    page.navigateTo();
    expect(page.getParagraphText()).toEqual('Welcome to app!!');
  });
});
